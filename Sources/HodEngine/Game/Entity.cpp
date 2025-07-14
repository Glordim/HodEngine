#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitNoSerialization.hpp>

namespace hod::game
{
	std::atomic<uint64_t> Entity::_nextInstanceId = 0;

	DESCRIBE_REFLECTED_CLASS(Entity, reflectionDescriptor)
	{
		ReflectionProperty* instanceIdProperty = AddPropertyT(reflectionDescriptor, &Entity::_instanceId, "_instanceId");
		instanceIdProperty->AddTrait<ReflectionTraitNoSerialization>();
		instanceIdProperty->AddTrait<ReflectionTraitHide>();

		AddPropertyT(reflectionDescriptor, &Entity::_localId, "_localId")->AddTrait<ReflectionTraitHide>();

		AddPropertyT(reflectionDescriptor, &Entity::_name, "_name");
		AddPropertyT(reflectionDescriptor, &Entity::_active, "_active");

		AddPropertyT(reflectionDescriptor, &Entity::_parent, "Parent", &Entity::SetParent)->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Entity::_children, "Children")->AddTrait<ReflectionTraitHide>();
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
		for (Component* component : _components)
		{
			DefaultAllocator::GetInstance().Delete(component);
		}
		_components.clear();
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
	void Entity::NotifyActivationChanged()
	{

	}

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

		return AddComponent(static_cast<Component*>(descriptor.CreateInstance()));
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

	/// @brief 
	/// @param component 
	void Entity::RemoveComponent(Component* component)
	{
		auto it = std::find(_components.begin(), _components.end(), component);
		if (it != _components.end())
		{
			_components.erase(it);
		}
	}

	/// @brief 
	/// @param descriptor 
	void Entity::RemoveComponent(const ReflectionDescriptor& descriptor)
	{
		uint32_t indexToRemove = -1;
		for (uint32_t index = 0; index < _components.size(); ++index)
		{
			Component* component = _components[index];
			if (&component->GetReflectionDescriptorV() == &descriptor)
			{
				_components.erase(_components.begin() + index);
				return;
			}

			if (component->GetReflectionDescriptorV().IsCompatible(descriptor) == true)
			{
				indexToRemove = index;
			}
		}
		if (indexToRemove != -1)
		{
			_components.erase(_components.begin() + indexToRemove);
		}
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
			for (uint32_t index = 0; index < parentLock->_children.size(); ++index)
			{
				if (parentLock->_children[index].Lock() == this)
				{
					return index;
				}
			}
		}
		return 0;
	}

	/// @brief 
	/// @param index 
	void Entity::SetSiblingIndex(uint32_t index)
	{
		Entity* parentLock = _parent.Lock();
		if (parentLock != nullptr)
		{
			uint32_t initialIndex = GetSiblingIndex();
			for (uint32_t i = initialIndex; i > index; --i)
			{
				parentLock->_children[i] = parentLock->_children[i - 1];
			}
			parentLock->_children[index] = WeakEntity(this);

			parentLock->_childrenChangedEvent.Emit();
		}
	}

	/// @brief 
	/// @param parent 
	void Entity::SetParent(const WeakEntity& parent)
	{
		if (_parent.Lock() == parent.Lock())
		{
			return;
		}

		Entity* previousParentLock = _parent.Lock();
		if (previousParentLock != nullptr)
		{
			auto itEnd = previousParentLock->_children.end();
			for (auto it = previousParentLock->_children.begin(); it != itEnd; ++it)
			{
				if (it->Lock() == this)
				{
					previousParentLock->_children.erase(it);
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
				newParentLock->_children.emplace_back(this);
			}
		}
		_parent = parent;

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
		//assert(_internalState != InternalState::Destructed);
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
