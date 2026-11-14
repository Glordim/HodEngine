#include <gtest/gtest.h>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Entity.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod
{
	/// @brief Counts its destructions
	class TrackedComponent : public Component
	{
		REFLECTED_CLASS(TrackedComponent, Component)

	public:
		~TrackedComponent() override
		{
			++_destroyedCount;
		}

		static inline int _destroyedCount = 0;
	};

	DESCRIBE_REFLECTED_CLASS(TrackedComponent, reflectionDescriptor) {}

	class EntityTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			TrackedComponent::_destroyedCount = 0;
			_entity = DefaultAllocator::GetInstance().New<Entity>("Entity");
		}

		void TearDown() override
		{
			DefaultAllocator::GetInstance().Delete(_entity);
		}

		Entity* _entity = nullptr;
	};

	TEST_F(EntityTest, RemoveComponentDeletesIt)
	{
		TrackedComponent* component = _entity->AddComponent<TrackedComponent>();
		ASSERT_NE(component, nullptr);
		ASSERT_EQ(_entity->GetComponents().Size(), 1u);

		_entity->RemoveComponent(component);
		EXPECT_EQ(_entity->GetComponents().Size(), 0u);
		EXPECT_EQ(TrackedComponent::_destroyedCount, 1);
	}

	TEST_F(EntityTest, RemoveComponentByTypeDeletesIt)
	{
		_entity->AddComponent<TrackedComponent>();

		_entity->RemoveComponent<TrackedComponent>();
		EXPECT_EQ(_entity->GetComponents().Size(), 0u);
		EXPECT_EQ(TrackedComponent::_destroyedCount, 1);
	}

	TEST_F(EntityTest, RemoveComponentNotOwnedDoesNothing)
	{
		Entity*           other = DefaultAllocator::GetInstance().New<Entity>("Other");
		TrackedComponent* component = other->AddComponent<TrackedComponent>();

		_entity->RemoveComponent(component);
		EXPECT_EQ(other->GetComponents().Size(), 1u);
		EXPECT_EQ(TrackedComponent::_destroyedCount, 0);

		DefaultAllocator::GetInstance().Delete(other);
		EXPECT_EQ(TrackedComponent::_destroyedCount, 1);
	}
}
