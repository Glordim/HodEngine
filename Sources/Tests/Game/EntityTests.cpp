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

	// Its entity's first, then down depth first: a grandchild of the first child before the second child
	TEST_F(EntityTest, ComponentGetsTheComponentsDownItsHierarchy)
	{
		TrackedComponent* tracked = _entity->AddComponent<TrackedComponent>();

		Entity* first = DefaultAllocator::GetInstance().New<Entity>("First");
		Entity* grandchild = DefaultAllocator::GetInstance().New<Entity>("Grandchild");
		Entity* second = DefaultAllocator::GetInstance().New<Entity>("Second");
		first->SetParent(_entity);
		grandchild->SetParent(first);
		second->SetParent(_entity);
		EXPECT_EQ(tracked->GetComponentInChildren<OtherComponent>(), nullptr);

		OtherComponent* secondOther = second->AddComponent<OtherComponent>();
		EXPECT_EQ(tracked->GetComponentInChildren<OtherComponent>(), secondOther);

		OtherComponent* grandchildOther = grandchild->AddComponent<OtherComponent>();
		EXPECT_EQ(tracked->GetComponentInChildren<OtherComponent>(), grandchildOther); // Depth first
		EXPECT_EQ(tracked->GetComponentInChildren(OtherComponent::GetReflectionDescriptor()), grandchildOther);

		OtherComponent* ownOther = _entity->AddComponent<OtherComponent>();
		EXPECT_EQ(tracked->GetComponentInChildren<OtherComponent>(), ownOther); // Its entity's first

		DefaultAllocator::GetInstance().Delete(second);
		DefaultAllocator::GetInstance().Delete(grandchild);
		DefaultAllocator::GetInstance().Delete(first);
	}

	// The children as Entity*, in their order: iterated, by index, copied
	TEST_F(EntityTest, ChildrenAsEntities)
	{
		EXPECT_TRUE(_entity->GetChildren().Empty());
		EXPECT_EQ(_entity->GetChildCount(), 0u);

		Entity* first = DefaultAllocator::GetInstance().New<Entity>("First");
		Entity* second = DefaultAllocator::GetInstance().New<Entity>("Second");
		first->SetParent(_entity);
		second->SetParent(_entity);

		ASSERT_EQ(_entity->GetChildCount(), 2u);
		EXPECT_EQ(_entity->GetChildren().Size(), 2u);
		EXPECT_EQ(_entity->GetChild(0), first);
		EXPECT_EQ(_entity->GetChildren()[1], second);

		Vector<Entity*> iterated;
		for (Entity* child : _entity->GetChildren())
		{
			iterated.PushBack(child);
		}
		ASSERT_EQ(iterated.Size(), 2u);
		EXPECT_EQ(iterated[0], first);
		EXPECT_EQ(iterated[1], second);

		// A copy: to iterate while they move
		Vector<Entity*> copy = _entity->GetChildren().ToVector();
		for (Entity* child : copy)
		{
			child->SetParent(WeakEntity(nullptr));
		}
		EXPECT_TRUE(_entity->GetChildren().Empty());

		DefaultAllocator::GetInstance().Delete(second);
		DefaultAllocator::GetInstance().Delete(first);
	}
}
