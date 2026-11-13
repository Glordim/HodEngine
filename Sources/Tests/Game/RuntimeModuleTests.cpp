#include <gtest/gtest.h>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/ComponentFactory.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/WorldSystem.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/RuntimeModule.hpp>
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
		bool                     g_failInit = false;

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

		void StartupA(RuntimeModuleRegistration& registration)
		{
			g_calls.push_back("StartupA");
			GameRegistrar gameRegistrar(registration);
			gameRegistrar.RegisterComponent<ModuleTestComponent>();
			gameRegistrar.RegisterWorldSystem(CountingWorldSystem::GetType());
			registration.AddInit(
				[]()
				{
					g_calls.push_back("InitA");
					return true;
				},
				[]() { g_calls.push_back(IsComponentRegistered() ? "ShutdownA, its types still there" : "ShutdownA, its types gone"); });
		}

		void StartupB(RuntimeModuleRegistration& registration)
		{
			g_calls.push_back("StartupB");
			registration.AddInit(
				[]()
				{
					g_calls.push_back("InitB");
					return g_failInit == false;
				},
				[]() { g_calls.push_back("ShutdownB"); });
		}

		void StartupDataStructs(RuntimeModuleRegistration& registration)
		{
			GameRegistrar gameRegistrar(registration);
			gameRegistrar.RegisterDataStruct<ModuleTestDataStruct>();
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
			g_failInit = false;
			ComponentFactory::CreateInstance();
			FrameSequencer::CreateInstance();
		}

		void TearDown() override
		{
			FrameSequencer::DestroyInstance();
			ComponentFactory::DestroyInstance();
		}
	};

	TEST_F(RuntimeModuleTest, StartupsThenInitsThenEachLibraryUndoneInReverse)
	{
		{
			RuntimeModuleHost host;
			host.Register("A", &StartupA);
			host.Register("B", &StartupB);
			EXPECT_TRUE(IsComponentRegistered());
			ASSERT_TRUE(host.InitAll());
		}
		EXPECT_FALSE(IsComponentRegistered());

		std::vector<std::string> expected {"StartupA", "StartupB", "InitA", "InitB", "ShutdownB", "ShutdownA, its types still there"};
		EXPECT_EQ(g_calls, expected);
	}

	TEST_F(RuntimeModuleTest, FailedInitStopsAndOnlyTheInitializedOnesShutDown)
	{
		g_failInit = true;
		RuntimeModuleHost host;
		host.Register("A", &StartupA);
		host.Register("B", &StartupB);
		EXPECT_FALSE(host.InitAll());

		std::vector<std::string> expected {"StartupA", "StartupB", "InitA", "InitB"};
		EXPECT_EQ(g_calls, expected);

		// B failed: not shut down, A was initialized: shut down
		host.UnregisterAll();
		expected.push_back("ShutdownA, its types still there");
		EXPECT_EQ(g_calls, expected);
		EXPECT_FALSE(IsComponentRegistered());
	}

	TEST_F(RuntimeModuleTest, WorldSystemsFollowTheirRegistration)
	{
		World* existingWorld = DefaultAllocator::GetInstance().New<World>();
		existingWorld->Init();
		EXPECT_EQ(existingWorld->FindSystem(CountingWorldSystem::GetType()), nullptr);

		{
			RuntimeModuleHost host;
			host.Register("A", &StartupA);

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
			RuntimeModuleHost host;
			host.Register("DataStructs", &StartupDataStructs);
			EXPECT_NE(DataStructFactory::GetInstance()->FindReflectionDescriptor(type), nullptr);
		}
		EXPECT_EQ(DataStructFactory::GetInstance()->FindReflectionDescriptor(type), nullptr);
		DataStructFactory::DestroyInstance();
	}
}
