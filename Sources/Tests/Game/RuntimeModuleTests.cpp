#include <gtest/gtest.h>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/ComponentFactory.hpp>
#include <HodEngine/Game/GameRegistrar.hpp>
#include <HodEngine/Game/RuntimeModule.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/WorldSystem.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
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

		void RegisterA(ModuleRegistration& registration)
		{
			g_calls.push_back("RegisterA");
			GameRegistrar gameRegistrar(registration);
			gameRegistrar.RegisterComponent<ModuleTestComponent>();
			gameRegistrar.RegisterWorldSystem(CountingWorldSystem::GetType());
		}

		bool InitA()
		{
			g_calls.push_back("InitA");
			return true;
		}

		void ShutdownA()
		{
			g_calls.push_back("ShutdownA");
		}

		void RegisterB(ModuleRegistration&)
		{
			g_calls.push_back("RegisterB");
		}

		bool InitB()
		{
			g_calls.push_back("InitB");
			return g_failInit == false;
		}

		void ShutdownB()
		{
			g_calls.push_back("ShutdownB");
		}

		void RegisterDataStructs(ModuleRegistration& registration)
		{
			GameRegistrar gameRegistrar(registration);
			gameRegistrar.RegisterDataStruct<ModuleTestDataStruct>();
		}

		const RuntimeModule g_moduleA {"A", &RegisterA, &InitA, &ShutdownA};
		const RuntimeModule g_moduleB {"B", &RegisterB, &InitB, &ShutdownB};
		const RuntimeModule g_moduleDataStructs {"DataStructs", &RegisterDataStructs};
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

		static bool IsComponentRegistered()
		{
			return ComponentFactory::GetInstance()->GetAllDescriptors().contains(ModuleTestComponent::GetReflectionDescriptor().GetType());
		}
	};

	TEST_F(RuntimeModuleTest, PhasesRunModuleByModuleThenShutdownInReverse)
	{
		{
			RuntimeModuleHost host;
			host.Register(g_moduleA);
			host.Register(g_moduleB);
			EXPECT_TRUE(IsComponentRegistered());
			ASSERT_TRUE(host.InitAll());
			host.ShutdownAll();
			EXPECT_TRUE(IsComponentRegistered()); // Unregistered with UnregisterAll only
		}
		EXPECT_FALSE(IsComponentRegistered());

		std::vector<std::string> expected {"RegisterA", "RegisterB", "InitA", "InitB", "ShutdownB", "ShutdownA"};
		EXPECT_EQ(g_calls, expected);
	}

	TEST_F(RuntimeModuleTest, FailedInitShutsDownTheInitializedOnes)
	{
		g_failInit = true;
		RuntimeModuleHost host;
		host.Register(g_moduleA);
		host.Register(g_moduleB);
		EXPECT_FALSE(host.InitAll());

		// B failed: not shut down, A was initialized: shut down
		std::vector<std::string> expected {"RegisterA", "RegisterB", "InitA", "InitB", "ShutdownA"};
		EXPECT_EQ(g_calls, expected);

		host.UnregisterAll();
		EXPECT_EQ(g_calls, expected); // Nothing shut down twice
	}

	TEST_F(RuntimeModuleTest, WorldSystemsFollowTheirRegistration)
	{
		World* existingWorld = DefaultAllocator::GetInstance().New<World>();
		existingWorld->Init();
		EXPECT_EQ(existingWorld->FindSystem(CountingWorldSystem::GetType()), nullptr);

		{
			RuntimeModuleHost host;
			host.Register(g_moduleA);

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
			host.Register(g_moduleDataStructs);
			EXPECT_NE(DataStructFactory::GetInstance()->FindReflectionDescriptor(type), nullptr);
		}
		EXPECT_EQ(DataStructFactory::GetInstance()->FindReflectionDescriptor(type), nullptr);
		DataStructFactory::DestroyInstance();
	}
}
