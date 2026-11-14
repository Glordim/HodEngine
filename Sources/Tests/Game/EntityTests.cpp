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

	DESCRIBE_REFLECTED_CLASS(TrackedComponent) {}

	/// @brief Another type, looked up from TrackedComponent
	class OtherComponent : public Component
	{
		REFLECTED_CLASS(OtherComponent, Component)
	};

	DESCRIBE_REFLECTED_CLASS(OtherComponent) {}

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

	// A component reaches its entity's components without going through GetOwner()
	TEST_F(EntityTest, ComponentGetsTheComponentsOfItsEntity)
	{
		TrackedComponent* tracked = _entity->AddComponent<TrackedComponent>();
		EXPECT_EQ(tracked->GetComponent<OtherComponent>(), nullptr);

		OtherComponent* other = _entity->AddComponent<OtherComponent>();
		EXPECT_EQ(tracked->GetComponent<OtherComponent>(), other);
		EXPECT_EQ(tracked->GetComponent(OtherComponent::GetReflectionDescriptor()), other);
		EXPECT_EQ(tracked->GetComponent<TrackedComponent>(), tracked);
	}

	TEST_F(EntityTest, ComponentGetsTheComponentsUpItsHierarchy)
	{
		OtherComponent* parentOther = _entity->AddComponent<OtherComponent>();

		Entity* child = DefaultAllocator::GetInstance().New<Entity>("Child");
		child->SetParent(_entity);
		TrackedComponent* tracked = child->AddComponent<TrackedComponent>();
		EXPECT_EQ(tracked->GetComponent<OtherComponent>(), nullptr);
		EXPECT_EQ(tracked->GetComponentInParent<OtherComponent>(), parentOther);
		EXPECT_EQ(tracked->GetComponentInParent(OtherComponent::GetReflectionDescriptor()), parentOther);

		OtherComponent* childOther = child->AddComponent<OtherComponent>();
		EXPECT_EQ(tracked->GetComponentInParent<OtherComponent>(), childOther); // Its entity's first

		DefaultAllocator::GetInstance().Delete(child);
	}
}
