#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitNoSerialization.hpp>

#undef max

namespace hod::inline game
{
	// From 1: 0 is "none" (WeakEntity, PickingManager::InvalidId: an entity of id 0 would not be pickable in the viewports)
	std::atomic<uint64_t> Entity::_nextInstanceId = 1;

	DESCRIBE_REFLECTED_ENUM(Entity::InternalState)
	{
		// constexpr auto names = EnumTrait::GetEnumNames<Entity::InternalState, 0, 1>();

		HOD_ENUM_VALUE(None);
		HOD_ENUM_VALUE(Constructed);
		HOD_ENUM_VALUE(Awaked);
		HOD_ENUM_VALUE(Started);
		HOD_ENUM_VALUE(Destructed);
	}

	DESCRIBE_REFLECTED_CLASS(Entity)
	{
		ReflectionProperty* instanceIdProperty = HOD_PROPERTY(_instanceId);
		instanceIdProperty->AddTrait<ReflectionTraitNoSerialization>();
		instanceIdProperty->AddTrait<ReflectionTraitHide>();

		HOD_PROPERTY(_localId)->AddTrait<ReflectionTraitHide>();

		HOD_PROPERTY(_name);
		HOD_PROPERTY(_active);

		HOD_PROPERTY(_parent, &Entity::SetParent)->AddTrait<ReflectionTraitHide>();
		HOD_PROPERTY(_children)->AddTrait<ReflectionTraitHide>();
	}

	/// @brief
	/// @param name
	Entity::Entity(const std::string_view& name)
	: _name(name)
	{
		_instanceId = _nextInstanceId.fetch_add(1);
	}

	/// @brief
	Entity::~Entity()
	{
		if (_scene != nullptr && _parent.Lock() == nullptr)
		{
			_scene->RemoveRoot(this);
		}

		for (Component* component : _components)
		{
			DefaultAllocator::GetInstance().Delete(component);
		}
		_components.Clear();
	}

	/// @brief
	/// @return
	uint64_t Entity::GetInstanceId() const
	{
		return _instanceId;
	}

	/// @brief
	/// @return
	uint64_t Entity::GetLocalId() const
	{
		return _localId;
	}

	/// @brief
	/// @param localId
	void Entity::SetLocalId(uint64_t localId)
	{
		_localId = localId;
	}

	/// @brief
	/// @return
	const String& Entity::GetName() const
	{
		return _name;
	}

	/// @brief
	/// @param name
	void Entity::SetName(const std::string_view& name)
	{
		_name = name;
	}

	/// @brief
	/// @param active
	void Entity::SetActive(bool active)
	{
		if (_active != active)
		{
			_active = active;

			if (_activeInHierarchy != _active)
			{
				ProcessActivation();
			}
		}
	}

	/// @brief
	/// @return
	bool Entity::GetActive() const
	{
		return _active;
	}

	/// @brief
	/// @return
	bool Entity::IsActiveInHierarchy() const
	{
		return _activeInHierarchy;
	}

	/// @brief
	void Entity::NotifyActivationChanged() {}

	/// @brief
	/// @return
	const Vector<Component*>& Entity::GetComponents() const
	{
		return _components;
	}

	/// @brief
	/// @param descriptor
	/// @return
	Component* Entity::GetComponent(const ReflectionDescriptor& descriptor)
	{
		for (Component* component : _components)
		{
			if (component->GetReflectionDescriptorV().IsCompatible(descriptor) == true)
			{
				return component;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param descriptor
	/// @return
	Component* Entity::GetComponentInParent(const ReflectionDescriptor& descriptor)
	{
		Component* component = GetComponent(descriptor);
		if (component != nullptr)
		{
			return component;
		}

		if (_parent.Lock() != nullptr)
		{
			return _parent.Lock()->GetComponentInParent(descriptor);
		}

		return nullptr;
	}

	/// @brief
	/// @param descriptor
	/// @return
	Component* Entity::AddComponent(const ReflectionDescriptor& descriptor)
	{
		Component* existingComponent = GetComponent(descriptor);
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}

		// Its requirements first: there before it in the list, constructed and awaked before it
		for (const ReflectionDescriptor* requiredComponent : GetMissingRequiredComponents(descriptor))
		{
			AddComponent(static_cast<Component*>(requiredComponent->CreateInstance()));
		}

		return AddComponent(static_cast<Component*>(descriptor.CreateInstance()));
	}

	/// @brief
	/// @param descriptor
	/// @return
	Component* Entity::AddComponentWithoutRequirements(const ReflectionDescriptor& descriptor)
	{
		Component* existingComponent = GetComponent(descriptor);
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}

		return AddComponent(static_cast<Component*>(descriptor.CreateInstance()));
	}

	/// @brief
	/// @param descriptor
	/// @return
	const Component* Entity::FindComponent(const ReflectionDescriptor& descriptor) const
	{
		for (const Component* component : _components)
		{
			if (component->GetReflectionDescriptorV().IsCompatible(descriptor) == true)
			{
				return component;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param descriptor
	/// @return
	Vector<const ReflectionDescriptor*> Entity::GetMissingRequiredComponents(const ReflectionDescriptor& descriptor) const
	{
		Vector<const ReflectionDescriptor*> missing;
		Vector<const ReflectionDescriptor*> visiting;
		CollectMissingRequiredComponents(descriptor, missing, visiting);
		return missing;
	}

	/// @brief Depth first: a required component's own requirements are listed before it
	/// @param descriptor
	/// @param missing
	/// @param visiting the types whose requirements are being collected: one of them required again (a cycle) is already going to be added
	void Entity::CollectMissingRequiredComponents(const ReflectionDescriptor& descriptor, Vector<const ReflectionDescriptor*>& missing,
	                                              Vector<const ReflectionDescriptor*>& visiting) const
	{
		visiting.PushBack(&descriptor);
		for (const ReflectionDescriptor* requiredComponent : ReflectionTraitRequireComponent::GetRequiredComponents(descriptor))
		{
			if (FindComponent(*requiredComponent) != nullptr)
			{
				continue;
			}

			bool alreadyPlanned = false;
			for (const ReflectionDescriptor* planned : missing)
			{
				alreadyPlanned |= planned->IsCompatible(*requiredComponent);
			}
			for (const ReflectionDescriptor* planned : visiting)
			{
				alreadyPlanned |= planned->IsCompatible(*requiredComponent);
			}
			if (alreadyPlanned)
			{
				continue;
			}

			if (requiredComponent->CanCreateInstance() == false)
			{
				OUTPUT_ERROR("Entity: {} requires {}, which can't be instantiated (require a concrete type)", descriptor.GetDisplayName(), requiredComponent->GetDisplayName());
				continue;
			}

			CollectMissingRequiredComponents(*requiredComponent, missing, visiting);
			missing.PushBack(requiredComponent);
		}
		visiting.PopBack();
	}

	/// @brief
	/// @param component
	/// @return
	Component* Entity::GetComponentRequiring(const Component* component) const
	{
		const ReflectionDescriptor& componentDescriptor = component->GetReflectionDescriptorV();
		for (Component* requiringComponent : _components)
		{
			if (requiringComponent == component)
			{
				continue;
			}

			for (const ReflectionDescriptor* requiredComponent : ReflectionTraitRequireComponent::GetRequiredComponents(requiringComponent->GetReflectionDescriptorV()))
			{
				if (componentDescriptor.IsCompatible(*requiredComponent) == false)
				{
					continue;
				}

				// Another one standing in for it (e.g. two node types deriving from the required one)
				bool replaced = false;
				for (const Component* other : _components)
				{
					replaced |= other != component && other->GetReflectionDescriptorV().IsCompatible(*requiredComponent);
				}
				if (replaced == false)
				{
					return requiringComponent;
				}
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param component
	/// @return
	Component* Entity::AddComponent(Component* component)
	{
		_components.push_back(component);
		component->AttachTo(this);

		if (_internalState != InternalState::None)
		{
			component->Construct();
			if (_activeInHierarchy == true)
			{
				bool playing = GetScene()->GetWorld()->GetEditorPlaying();

				if (playing)
				{
					component->Awake();
				}
				component->Enable();
				if (playing)
				{
					component->Start();
				}
			}
		}

		return component;
	}

	/// @brief Ends its life as the entity's destruction would (disabled, destructed) and deletes it
	/// @param component
	bool Entity::RemoveComponent(Component* component)
	{
		auto it = std::find(_components.Begin(), _components.End(), component);
		if (it == _components.End())
		{
			return false;
		}

		Component* requiringComponent = GetComponentRequiring(component);
		if (requiringComponent != nullptr)
		{
			OUTPUT_ERROR("Entity '{}': can't remove {}, {} requires it", _name, component->GetReflectionDescriptorV().GetDisplayName(),
			             requiringComponent->GetReflectionDescriptorV().GetDisplayName());
			return false;
		}
		_components.Erase(it);

		if (component->IsEnabledInHierarchy() == true)
		{
			component->Disable();
		}
		if (component->GetInternalState() != Component::InternalState::None && component->GetInternalState() != Component::InternalState::Destructed)
		{
			component->Destruct();
		}
		DefaultAllocator::GetInstance().Delete(component);
		return true;
	}

	/// @brief The one of this exact type, otherwise the last one deriving from it
	/// @param descriptor
	bool Entity::RemoveComponent(const ReflectionDescriptor& descriptor)
	{
		Component* componentToRemove = nullptr;
		for (Component* component : _components)
		{
			// Compared by type: each DLL has its own descriptor instance in shared builds
			if (component->GetReflectionDescriptorV().GetType() == descriptor.GetType())
			{
				componentToRemove = component;
				break;
			}

			if (component->GetReflectionDescriptorV().IsCompatible(descriptor) == true)
			{
				componentToRemove = component;
			}
		}
		if (componentToRemove == nullptr)
		{
			return false;
		}
		return RemoveComponent(componentToRemove);
	}

	/// @brief
	/// @param prefab
	void Entity::SetPrefabResource(std::shared_ptr<PrefabResource> prefabResource)
	{
		_prefabResource = prefabResource;
	}

	/// @brief
	/// @return
	std::shared_ptr<PrefabResource> Entity::GetPrefabResource() const
	{
		return _prefabResource;
	}

	/// @brief
	/// @return
	const Vector<WeakEntity>& Entity::GetChildren() const
	{
		return _children;
	}

	/// @brief
	/// @return
	uint32_t Entity::GetSiblingIndex() const
	{
		Entity* parentLock = _parent.Lock();
		if (parentLock != nullptr)
		{
			for (uint32_t index = 0; index < parentLock->_children.Size(); ++index)
			{
				if (parentLock->_children[index].Lock() == this)
				{
					return index;
				}
			}
		}
		else if (_scene != nullptr)
		{
			return _scene->GetRootIndex(this);
		}
		return 0;
	}

	/// @brief Among the children of its parent, or the roots of its scene
	/// @param index
	void Entity::SetSiblingIndex(uint32_t index)
	{
		Entity* parentLock = _parent.Lock();
		if (parentLock == nullptr)
		{
			if (_scene != nullptr)
			{
				_scene->SetRootIndex(this, index);
			}
		}
		else
		{
			// Moved towards the start or the end: the siblings in between shift by one the other way
			uint32_t initialIndex = GetSiblingIndex();
			index = Min(index, (uint32_t)parentLock->_children.Size() - 1);
			if (initialIndex == index)
			{
				return;
			}
			for (uint32_t i = initialIndex; i > index; --i)
			{
				parentLock->_children[i] = parentLock->_children[i - 1];
			}
			for (uint32_t i = initialIndex; i < index; ++i)
			{
				parentLock->_children[i] = parentLock->_children[i + 1];
			}
			parentLock->_children[index] = WeakEntity(this);

			parentLock->_childrenChangedEvent.Emit();
		}
	}

	/// @brief
	/// @param parent
	void Entity::SetParent(const WeakEntity& parent)
	{
		assert(parent.Lock() != this);

		if (_parent.Lock() == parent.Lock())
		{
			return;
		}

		Entity* previousParentLock = _parent.Lock();
		if (previousParentLock != nullptr)
		{
			auto itEnd = previousParentLock->_children.End();
			for (auto it = previousParentLock->_children.Begin(); it != itEnd; ++it)
			{
				if (it->Lock() == this)
				{
					previousParentLock->_children.Erase(it);
					break;
				}
			}
			// todo assert
		}

		Entity* newParentLock = parent.Lock();
		if (newParentLock != nullptr)
		{
			bool exist = false;
			for (const WeakEntity& child : newParentLock->_children)
			{
				if (child.Lock() == this)
				{
					exist = true;
					break;
				}
			}
			if (exist == false)
			{
				newParentLock->_children.EmplaceBack(this);
			}
		}
		_parent = parent;

		// Among its scene's roots (see Scene::GetRootEntities) when it has no parent, the last one when it gets there
		if (_scene != nullptr)
		{
			if (previousParentLock == nullptr && newParentLock != nullptr)
			{
				_scene->RemoveRoot(this);
			}
			else if (previousParentLock != nullptr && newParentLock == nullptr)
			{
				_scene->AddRoot(this);
			}
		}

		if (_internalState != InternalState::None)
		{
			ProcessActivation();
		}

		if (previousParentLock)
		{
			previousParentLock->_childrenChangedEvent.Emit();
		}
		if (newParentLock)
		{
			newParentLock->_childrenChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const WeakEntity& Entity::GetParent() const
	{
		return _parent;
	}

	Entity::ChildrenChangedEvent& Entity::GetChildrenChangedEvent()
	{
		return _childrenChangedEvent;
	}

	/// @brief
	/// @param scene
	void Entity::SetScene(Scene* scene)
	{
		if (_scene == scene)
		{
			return;
		}

		// A root is one of its scene's roots (see Scene::GetRootEntities)
		if (_parent.Lock() == nullptr)
		{
			if (_scene != nullptr)
			{
				_scene->RemoveRoot(this);
			}
			if (scene != nullptr)
			{
				scene->AddRoot(this);
			}
		}
		_scene = scene;
	}

	/// @brief
	/// @return
	Scene* Entity::GetScene() const
	{
		return _scene;
	}

	/// @brief
	void Entity::ProcessActivation()
	{
		Construct();

		if (_active && (_parent.Lock() == nullptr || _parent.Lock()->IsActiveInHierarchy()))
		{
			if (GetScene()->GetWorld()->GetEditorPlaying())
			{
				Awake();
			}

			Enable();

			if (GetScene()->GetWorld()->GetEditorPlaying())
			{
				Start();
			}
		}
		else
		{
			if (_activeInHierarchy == true)
			{
				Disable();
			}
		}
	}

	/// @brief
	/// @return
	Entity::InternalState Entity::GetInternalState() const
	{
		return _internalState;
	}

	/// @brief
	void Entity::Construct()
	{
		for (Component* component : _components)
		{
			component->Construct();
		}

		_internalState = InternalState::Constructed;

		for (const WeakEntity& child : _children)
		{
			Entity* childEntity = child.Lock();
			childEntity->Construct();
		}
	}

	/// @brief
	void Entity::Awake()
	{
		assert(_active == true);

		for (Component* component : _components)
		{
			component->Awake();
		}

		_internalState = InternalState::Awaked;

		for (const WeakEntity& child : _children)
		{
			Entity* childEntity = child.Lock();
			if (childEntity->GetActive() == true)
			{
				childEntity->Awake();
			}
		}
	}

	/// @brief
	void Entity::Enable()
	{
		if (_activeInHierarchy == true)
		{
			return;
		}

		for (Component* component : _components)
		{
			if (component->GetEnabled())
			{
				component->Enable();
			}
		}

		_activeInHierarchy = true;

		for (const WeakEntity& child : _children)
		{
			Entity* childEntity = child.Lock();
			if (childEntity->GetActive() == true)
			{
				childEntity->Enable();
			}
		}
	}

	/// @brief
	void Entity::Start()
	{
		assert(_active == true);

		for (Component* component : _components)
		{
			component->Start();
		}

		_internalState = InternalState::Started;

		for (const WeakEntity& child : _children)
		{
			Entity* childEntity = child.Lock();
			if (childEntity->GetActive() == true)
			{
				childEntity->Start();
			}
		}
	}

	/// @brief
	void Entity::Disable()
	{
		if (_activeInHierarchy == false)
		{
			return;
		}

		for (const WeakEntity& child : _children)
		{
			Entity* childEntity = child.Lock();
			if (childEntity->GetActive() == true)
			{
				childEntity->Disable();
			}
		}

		for (Component* component : _components)
		{
			component->Disable();
		}

		_activeInHierarchy = false;
	}

	/// @brief
	void Entity::Destruct()
	{
		// assert(_internalState != InternalState::Destructed);
		if (_internalState == InternalState::Destructed)
		{
			return;
		}

		for (const WeakEntity& child : _children)
		{
			Entity* childEntity = child.Lock();
			childEntity->Destruct();
		}

		for (Component* component : _components)
		{
			component->Destruct();
		}

		_internalState = InternalState::Destructed;
	}
}
