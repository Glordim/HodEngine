#include <gtest/gtest.h>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/ComponentFactory.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/WorldSystem.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/ModuleHost.hpp>
#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>

#include <string>
#include <vector>

namespace hod
{
	class ModuleTestComponent : public Component
	{
		REFLECTED_CLASS(ModuleTestComponent, Component)
	};

	class ModuleTestDataStruct : public DataStruct
	{
		REFLECTED_CLASS(ModuleTestDataStruct, DataStruct)
	};

	namespace
	{
		std::vector<std::string> g_calls;
		bool                     g_failStart = false;

		class CountingWorldSystem : public WorldSystem
		{
		public:
			explicit CountingWorldSystem(World& world)
			: WorldSystem(world)
			{
				++_liveCount;
			}

			~CountingWorldSystem() override
			{
				--_liveCount;
			}

			static const WorldSystemType& GetType()
			{
				static const WorldSystemType type {"Counting", +[](World& world) -> WorldSystem* { return DefaultAllocator::GetInstance().New<CountingWorldSystem>(world); }};
				return type;
			}

			static inline int _liveCount = 0;
		};

		bool IsComponentRegistered()
		{
			return ComponentFactory::GetInstance()->GetAllDescriptors().contains(ModuleTestComponent::GetReflectionDescriptor().GetType());
		}

		bool StartupA(ModuleRegistration& registration)
		{
			g_calls.push_back("StartA");
			GameRegistrar gameRegistrar(registration);
			gameRegistrar.RegisterComponent<ModuleTestComponent>();
			gameRegistrar.RegisterWorldSystem(CountingWorldSystem::GetType());
			// Its service, stopped before its types go
			registration.AddUnregister([]() { g_calls.push_back(IsComponentRegistered() ? "StopA, its types still there" : "StopA, its types gone"); });
			return true;
		}

		bool StartupB(ModuleRegistration& registration)
		{
			g_calls.push_back("StartB");
			registration.AddUnregister([]() { g_calls.push_back("StopB"); });
			return g_failStart == false;
		}

		bool StartupDataStructs(ModuleRegistration& registration)
		{
			GameRegistrar gameRegistrar(registration);
			gameRegistrar.RegisterDataStruct<ModuleTestDataStruct>();
			return true;
		}
	}

	DESCRIBE_REFLECTED_CLASS(ModuleTestComponent, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	DESCRIBE_REFLECTED_CLASS(ModuleTestDataStruct, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	class RuntimeModuleTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			g_calls.clear();
			g_failStart = false;
			ComponentFactory::CreateInstance();
			FrameSequencer::CreateInstance();
		}

		void TearDown() override
		{
			FrameSequencer::DestroyInstance();
			ComponentFactory::DestroyInstance();
		}
	};

	TEST_F(RuntimeModuleTest, StartedInOrderStoppedInReverse)
	{
		{
			ModuleHost host;
			EXPECT_TRUE(host.Start("A", &StartupA));
			EXPECT_TRUE(host.Start("B", &StartupB));
			EXPECT_TRUE(IsComponentRegistered());
			EXPECT_EQ(host.GetModuleCount(), 2u);
		}
		EXPECT_FALSE(IsComponentRegistered());

		std::vector<std::string> expected {"StartA", "StartB", "StopB", "StopA, its types still there"};
		EXPECT_EQ(g_calls, expected);
	}

	TEST_F(RuntimeModuleTest, FailedStartIsUndoneAtOnce)
	{
		g_failStart = true;
		ModuleHost host;
		EXPECT_TRUE(host.Start("A", &StartupA));
		EXPECT_FALSE(host.Start("B", &StartupB));

		// What B registered before failing undone right away, B not kept
		std::vector<std::string> expected {"StartA", "StartB", "StopB"};
		EXPECT_EQ(g_calls, expected);
		EXPECT_EQ(host.GetModuleCount(), 1u);

		// The rest of its module (A) stopped by the caller
		host.StopLast(1);
		expected.push_back("StopA, its types still there");
		EXPECT_EQ(g_calls, expected);
		EXPECT_FALSE(IsComponentRegistered());
		EXPECT_EQ(host.GetModuleCount(), 0u);
	}

	TEST_F(RuntimeModuleTest, LoadedFromItsLibraryUnloadedOnceStopped)
	{
		Path modulesDirectory = FileSystem::GetExecutablePath().ParentPath();
		ModuleHost host;

		// A real one: Physics, a plugin base (it starts the physics engine, doesn't need Game)
		EXPECT_TRUE(host.Load("Physics", modulesDirectory / "Physics"));
		EXPECT_EQ(host.GetModuleCount(), 1u);

		// Not kept: a library without the entry point (Core isn't a module), one that doesn't exist
		EXPECT_FALSE(host.Load("Core", modulesDirectory / "Core"));
		EXPECT_FALSE(host.Load("Missing", modulesDirectory / "NoSuchModule"));
		EXPECT_EQ(host.GetModuleCount(), 1u);

		host.StopAll();
		EXPECT_EQ(host.GetModuleCount(), 0u);
	}

	TEST_F(RuntimeModuleTest, WorldSystemsFollowTheirRegistration)
	{
		World* existingWorld = DefaultAllocator::GetInstance().New<World>();
		existingWorld->Init();
		EXPECT_EQ(existingWorld->FindSystem(CountingWorldSystem::GetType()), nullptr);

		{
			ModuleHost host;
			host.Start("A", &StartupA);

			// Added to the worlds that existed already, and to the new ones
			EXPECT_NE(existingWorld->FindSystem(CountingWorldSystem::GetType()), nullptr);
			World* newWorld = DefaultAllocator::GetInstance().New<World>();
			newWorld->Init();
			EXPECT_NE(newWorld->FindSystem(CountingWorldSystem::GetType()), nullptr);
			EXPECT_EQ(CountingWorldSystem::_liveCount, 2);

			DefaultAllocator::GetInstance().Delete(newWorld);
			EXPECT_EQ(CountingWorldSystem::_liveCount, 1);
		}

		// The module went away: its systems too
		EXPECT_EQ(existingWorld->FindSystem(CountingWorldSystem::GetType()), nullptr);
		EXPECT_EQ(CountingWorldSystem::_liveCount, 0);
		DefaultAllocator::GetInstance().Delete(existingWorld);
	}

	TEST_F(RuntimeModuleTest, DataStructsFollowTheirRegistration)
	{
		DataStructFactory::CreateInstance();
		const RttiType type = ModuleTestDataStruct::GetReflectionDescriptor().GetType();
		{
			ModuleHost host;
			host.Start("DataStructs", &StartupDataStructs);
			EXPECT_NE(DataStructFactory::GetInstance()->FindReflectionDescriptor(type), nullptr);
		}
		EXPECT_EQ(DataStructFactory::GetInstance()->FindReflectionDescriptor(type), nullptr);
		DataStructFactory::DestroyInstance();
	}
}
