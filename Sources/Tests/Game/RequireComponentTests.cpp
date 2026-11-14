#include <gtest/gtest.h>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/ComponentFactory.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/RequiredComponent.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/SceneSerializer.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod
{
	/// @brief Required by the others
	class RequiredBaseComponent : public Component
	{
		REFLECTED_CLASS(RequiredBaseComponent, Component)
	};

	DESCRIBE_REFLECTED_CLASS(RequiredBaseComponent) {}

	/// @brief Stands in for RequiredBaseComponent
	class RequiredDerivedComponent : public RequiredBaseComponent
	{
		REFLECTED_CLASS(RequiredDerivedComponent, RequiredBaseComponent)
	};

	DESCRIBE_REFLECTED_CLASS(RequiredDerivedComponent) {}

	class NeedsBaseComponent : public Component
	{
		REFLECTED_CLASS(NeedsBaseComponent, Component)
	};

	DESCRIBE_REFLECTED_CLASS(NeedsBaseComponent)
	{
		HOD_TRAIT(ReflectionTraitRequireComponent, &RequiredBaseComponent::GetReflectionDescriptor);
	}

	/// @brief Inherits its parent's requirement
	class NeedsBaseDerivedComponent : public NeedsBaseComponent
	{
		REFLECTED_CLASS(NeedsBaseDerivedComponent, NeedsBaseComponent)
	};

	DESCRIBE_REFLECTED_CLASS(NeedsBaseDerivedComponent) {}

	/// @brief Requires one requiring another
	class NeedsNeedsBaseComponent : public Component
	{
		REFLECTED_CLASS(NeedsNeedsBaseComponent, Component)
	};

	DESCRIBE_REFLECTED_CLASS(NeedsNeedsBaseComponent)
	{
		HOD_TRAIT(ReflectionTraitRequireComponent, &NeedsBaseComponent::GetReflectionDescriptor);
	}

	class CycleBComponent;

	/// @brief CycleA and CycleB require each other
	class CycleAComponent : public Component
	{
		REFLECTED_CLASS(CycleAComponent, Component)
	};

	class CycleBComponent : public Component
	{
		REFLECTED_CLASS(CycleBComponent, Component)
	};

	DESCRIBE_REFLECTED_CLASS(CycleAComponent)
	{
		HOD_TRAIT(ReflectionTraitRequireComponent, &CycleBComponent::GetReflectionDescriptor);
	}

	DESCRIBE_REFLECTED_CLASS(CycleBComponent)
	{
		HOD_TRAIT(ReflectionTraitRequireComponent, &CycleAComponent::GetReflectionDescriptor);
	}

	/// @brief Requires RequiredBaseComponent through its member
	class CachesBaseComponent : public Component
	{
		REFLECTED_CLASS(CachesBaseComponent, Component)

	public:
		void OnConstruct() override
		{
			_seenInOnConstruct = _base.Get();
		}

		RequiredComponent<RequiredBaseComponent> _base;
		RequiredBaseComponent*                   _seenInOnConstruct = nullptr;
	};

	DESCRIBE_REFLECTED_CLASS(CachesBaseComponent)
	{
		HOD_REQUIRED_COMPONENT(_base);
	}

	/// @brief Its parent's member filled too
	class CachesBaseDerivedComponent : public CachesBaseComponent
	{
		REFLECTED_CLASS(CachesBaseDerivedComponent, CachesBaseComponent)
	};

	DESCRIBE_REFLECTED_CLASS(CachesBaseDerivedComponent) {}

	class RequireComponentTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			_entity = DefaultAllocator::GetInstance().New<Entity>("Entity");
		}

		void TearDown() override
		{
			DefaultAllocator::GetInstance().Delete(_entity);
		}

		static uint64_t TypeAt(const Entity* entity, uint32_t index)
		{
			return entity->GetComponents()[index]->GetReflectionDescriptorV().GetType();
		}

		Entity* _entity = nullptr;
	};

	TEST_F(RequireComponentTest, AddsTheRequiredComponentFirst)
	{
		NeedsBaseComponent* component = _entity->AddComponent<NeedsBaseComponent>();
		ASSERT_NE(component, nullptr);
		ASSERT_EQ(_entity->GetComponents().Size(), 2u);
		EXPECT_EQ(TypeAt(_entity, 0), RequiredBaseComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(_entity->GetComponents()[1], component);
	}

	TEST_F(RequireComponentTest, AddsRecursivelyDependenciesFirst)
	{
		_entity->AddComponent<NeedsNeedsBaseComponent>();
		ASSERT_EQ(_entity->GetComponents().Size(), 3u);
		EXPECT_EQ(TypeAt(_entity, 0), RequiredBaseComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(TypeAt(_entity, 1), NeedsBaseComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(TypeAt(_entity, 2), NeedsNeedsBaseComponent::GetReflectionDescriptor().GetType());
	}

	TEST_F(RequireComponentTest, InheritsTheParentsRequirement)
	{
		_entity->AddComponent<NeedsBaseDerivedComponent>();
		ASSERT_EQ(_entity->GetComponents().Size(), 2u);
		EXPECT_EQ(TypeAt(_entity, 0), RequiredBaseComponent::GetReflectionDescriptor().GetType());
	}

	TEST_F(RequireComponentTest, DerivedTypeSatisfiesTheRequirement)
	{
		_entity->AddComponent<RequiredDerivedComponent>();
		_entity->AddComponent<NeedsBaseComponent>();
		ASSERT_EQ(_entity->GetComponents().Size(), 2u);
		EXPECT_EQ(TypeAt(_entity, 0), RequiredDerivedComponent::GetReflectionDescriptor().GetType());
	}

	TEST_F(RequireComponentTest, CycleAddsEachOnce)
	{
		_entity->AddComponent<CycleAComponent>();
		ASSERT_EQ(_entity->GetComponents().Size(), 2u);
		EXPECT_EQ(TypeAt(_entity, 0), CycleBComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(TypeAt(_entity, 1), CycleAComponent::GetReflectionDescriptor().GetType());
	}

	TEST_F(RequireComponentTest, RequiredComponentCantBeRemoved)
	{
		NeedsBaseComponent* needsBase = _entity->AddComponent<NeedsBaseComponent>();
		Component*          required = _entity->GetComponent<RequiredBaseComponent>();
		ASSERT_NE(required, nullptr);

		EXPECT_EQ(_entity->GetComponentRequiring(required), needsBase);
		EXPECT_FALSE(_entity->RemoveComponent(required));
		EXPECT_FALSE(_entity->RemoveComponent<RequiredBaseComponent>());
		EXPECT_EQ(_entity->GetComponents().Size(), 2u);

		// Once nothing requires it any more
		EXPECT_TRUE(_entity->RemoveComponent(needsBase));
		EXPECT_EQ(_entity->GetComponentRequiring(required), nullptr);
		EXPECT_TRUE(_entity->RemoveComponent(required));
		EXPECT_EQ(_entity->GetComponents().Size(), 0u);
	}

	TEST_F(RequireComponentTest, RequiredComponentRemovableWhenAnotherStandsIn)
	{
		_entity->AddComponent<NeedsBaseComponent>();
		Component* required = _entity->GetComponent<RequiredBaseComponent>();
		_entity->AddComponent<RequiredDerivedComponent>(); // the base is not a derived: added
		ASSERT_EQ(_entity->GetComponents().Size(), 3u);

		EXPECT_EQ(_entity->GetComponentRequiring(required), nullptr);
		EXPECT_TRUE(_entity->RemoveComponent(required));
		Component* standIn = _entity->GetComponent<RequiredBaseComponent>();
		ASSERT_NE(standIn, nullptr);
		EXPECT_NE(_entity->GetComponentRequiring(standIn), nullptr); // the last one now
		EXPECT_FALSE(_entity->RemoveComponent(standIn));
	}

	TEST_F(RequireComponentTest, MissingRequiredComponents)
	{
		Vector<const ReflectionDescriptor*> missing = _entity->GetMissingRequiredComponents(NeedsNeedsBaseComponent::GetReflectionDescriptor());
		ASSERT_EQ(missing.Size(), 2u);
		EXPECT_EQ(missing[0]->GetType(), RequiredBaseComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(missing[1]->GetType(), NeedsBaseComponent::GetReflectionDescriptor().GetType());

		_entity->AddComponent<RequiredBaseComponent>();
		missing = _entity->GetMissingRequiredComponents(NeedsNeedsBaseComponent::GetReflectionDescriptor());
		ASSERT_EQ(missing.Size(), 1u);
		EXPECT_EQ(missing[0]->GetType(), NeedsBaseComponent::GetReflectionDescriptor().GetType());
	}

	/// @brief Loading adds what the components written require and is missing (a requirement declared since the data was saved), after them
	TEST(RequireComponentLoadTest, LoadingAddsTheMissingRequirements)
	{
		ComponentFactory::CreateInstance();
		ComponentFactory::GetInstance()->Register<RequiredBaseComponent>();
		ComponentFactory::GetInstance()->Register<NeedsBaseComponent>();

		Document      source;
		DocumentNode& entityNode = source.GetRootNode().AddChild("");
		entityNode.AddChild("_localId").SetUInt64(1);
		entityNode.AddChild("_name").SetString("Entity");
		DocumentNode& componentNode = entityNode.AddChild("Components").AddChild("");
		componentNode.AddChild("MetaType").SetUInt64(NeedsBaseComponent::GetReflectionDescriptor().GetType());
		componentNode.AddChild("_localId").SetUInt64(2);

		SceneSerializer loader;
		ASSERT_TRUE(loader.Deserialize(source.GetRootNode()));
		ASSERT_EQ(loader.GetEntities().Size(), 1u);
		Entity* entity = loader.GetEntities()[0];
		ASSERT_EQ(entity->GetComponents().Size(), 2u);
		EXPECT_EQ(entity->GetComponents()[0]->GetReflectionDescriptorV().GetType(), NeedsBaseComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(entity->GetComponents()[1]->GetReflectionDescriptorV().GetType(), RequiredBaseComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(entity->GetComponents()[0]->GetLocalId(), 2u);
		EXPECT_EQ(entity->GetComponents()[1]->GetLocalId(), 0u); // Given one when saved
		EXPECT_TRUE(entity->GetMissingRequiredComponents(NeedsBaseComponent::GetReflectionDescriptor()).Empty());

		DefaultAllocator::GetInstance().Delete(entity);
		ComponentFactory::DestroyInstance();
	}

	class RequiredComponentTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Not initialized: no frame job nor systems needed to construct the entities
			_world = DefaultAllocator::GetInstance().New<World>();
			_scene = DefaultAllocator::GetInstance().New<Scene>();
			_world->AddScene(_scene);
		}

		void TearDown() override
		{
			_world->RemoveScene(_scene);
			DefaultAllocator::GetInstance().Delete(_scene);
			DefaultAllocator::GetInstance().Delete(_world);
		}

		World* _world = nullptr;
		Scene* _scene = nullptr;
	};

	// The member is the requirement: added with it, then filled before its OnConstruct
	TEST_F(RequiredComponentTest, MemberIsTheRequirementFilledBeforeOnConstruct)
	{
		Entity*              entity = _scene->CreateEntity("Entity");
		CachesBaseComponent* caches = entity->AddComponent<CachesBaseComponent>();
		ASSERT_NE(caches, nullptr);

		RequiredBaseComponent* base = entity->GetComponent<RequiredBaseComponent>();
		ASSERT_NE(base, nullptr);
		EXPECT_EQ(caches->_base.Get(), base);
		EXPECT_EQ(caches->_seenInOnConstruct, base);
		EXPECT_EQ(entity->GetComponentRequiring(base), caches);

		// Not a property: neither serialized nor shown
		EXPECT_EQ(CachesBaseComponent::GetReflectionDescriptor().FindProperty("_base"), nullptr);
	}

	TEST_F(RequiredComponentTest, StandInAndParentMember)
	{
		Entity*                     entity = _scene->CreateEntity("Entity");
		RequiredDerivedComponent*   standIn = entity->AddComponent<RequiredDerivedComponent>();
		CachesBaseDerivedComponent* caches = entity->AddComponent<CachesBaseDerivedComponent>();
		ASSERT_EQ(entity->GetComponents().Size(), 2u);
		EXPECT_EQ(caches->_base.Get(), standIn);
	}
}
