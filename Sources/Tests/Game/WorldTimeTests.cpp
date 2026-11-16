#include <gtest/gtest.h>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/WorldTime.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod::world_time_tests
{
	/// @return The fixed steps run after an Advance
	static uint32_t CountFixedSteps(WorldTime& time)
	{
		uint32_t count = 0;
		while (time.ConsumeFixedStep())
		{
			++count;
		}
		return count;
	}

	TEST(WorldTime, AdvanceAccumulates)
	{
		WorldTime time;
		time.Advance(0.05f);
		time.Advance(0.03f);

		EXPECT_FLOAT_EQ(time.GetDeltaTime(), 0.03f);
		EXPECT_FLOAT_EQ(time.GetUnscaledDeltaTime(), 0.03f);
		EXPECT_NEAR(time.GetElapsed(), 0.08, 1e-6);
		EXPECT_NEAR(time.GetUnscaledElapsed(), 0.08, 1e-6);
		EXPECT_EQ(time.GetFrameCount(), 2u);
	}

	TEST(WorldTime, TimeScaleScalesTheGameplayOnly)
	{
		WorldTime time;
		time.SetTimeScale(0.5f);
		time.Advance(0.05f);

		EXPECT_FLOAT_EQ(time.GetDeltaTime(), 0.025f);
		EXPECT_FLOAT_EQ(time.GetUnscaledDeltaTime(), 0.05f);
		EXPECT_NEAR(time.GetElapsed(), 0.025, 1e-6);
		EXPECT_NEAR(time.GetUnscaledElapsed(), 0.05, 1e-6);
	}

	TEST(WorldTime, TimeScaleZeroPauses)
	{
		WorldTime time;
		time.Advance(0.05f);
		CountFixedSteps(time);
		double elapsed = time.GetElapsed();

		time.SetTimeScale(0.0f);
		for (int frame = 0; frame < 3; ++frame)
		{
			time.Advance(0.05f);
			EXPECT_EQ(time.GetDeltaTime(), 0.0f);
			EXPECT_EQ(CountFixedSteps(time), 0u);
		}

		EXPECT_EQ(time.GetElapsed(), elapsed);
		EXPECT_NEAR(time.GetUnscaledElapsed(), 0.2, 1e-6);
		EXPECT_EQ(time.GetFrameCount(), 4u);
	}

	TEST(WorldTime, NegativeTimeScaleIsZero)
	{
		WorldTime time;
		time.SetTimeScale(-2.0f);
		EXPECT_EQ(time.GetTimeScale(), 0.0f);
	}

	TEST(WorldTime, DeltaCapped)
	{
		WorldTime time;
		time.SetMaxDeltaTime(0.1f);
		time.Advance(5.0f); // A hitch, a breakpoint

		EXPECT_FLOAT_EQ(time.GetDeltaTime(), 0.1f);
		EXPECT_FLOAT_EQ(time.GetUnscaledDeltaTime(), 0.1f);
		uint32_t steps = CountFixedSteps(time);
		EXPECT_GE(steps, 11u); // 0.1s of fixed steps, not 5s
		EXPECT_LE(steps, 12u);

		time.Advance(-1.0f);
		EXPECT_EQ(time.GetDeltaTime(), 0.0f);
	}

	TEST(WorldTime, FixedStepsCarryTheRemainder)
	{
		WorldTime time;
		float     step = time.GetFixedTimestep();

		time.Advance(step * 3.4f);
		EXPECT_EQ(CountFixedSteps(time), 3u);

		time.Advance(step * 0.7f); // With the 0.4 left: one more
		EXPECT_EQ(CountFixedSteps(time), 1u);
	}

	TEST(WorldTime, FixedStepsFollowTheTimeScale)
	{
		WorldTime time;
		float     step = time.GetFixedTimestep();

		time.SetTimeScale(2.0f);
		time.Advance(step * 2.6f);
		EXPECT_EQ(CountFixedSteps(time), 5u);

		time.SetTimeScale(0.5f);
		time.Advance(step * 2.6f); // 1.3 + the 0.2 left
		EXPECT_EQ(CountFixedSteps(time), 1u);
	}

	/// @brief Counts its updates
	class CountingComponent : public Component
	{
		REFLECTED_CLASS(CountingComponent, Component)

	public:
		void OnUpdate(float deltaTime) override
		{
			(void)deltaTime;
			++_updates;
		}

		void OnFixedUpdate() override
		{
			++_fixedUpdates;
		}

		int _updates = 0;
		int _fixedUpdates = 0;
	};

	DESCRIBE_REFLECTED_CLASS(CountingComponent) {}

	TEST(WorldTime, DisabledComponentNotUpdated)
	{
		World world;
		world.SetEditorPlaying(true);
		Scene* scene = DefaultAllocator::GetInstance().New<Scene>();
		world.AddScene(scene);

		Entity*            entity = scene->CreateEntity("Entity");
		CountingComponent* component = entity->AddComponent<CountingComponent>();

		scene->Update(0.01f);
		scene->FixedUpdate();
		EXPECT_EQ(component->_updates, 1);
		EXPECT_EQ(component->_fixedUpdates, 1);

		component->SetEnabled(false);
		scene->Update(0.01f);
		scene->FixedUpdate();
		EXPECT_EQ(component->_updates, 1);
		EXPECT_EQ(component->_fixedUpdates, 1);
	}
}
