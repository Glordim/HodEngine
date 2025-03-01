#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Component.hpp"
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
		//AddPropertyT(reflectionDescriptor, &Entity::_children, "Children")->AddTrait<ReflectionTraitHide>();
	}

	/// @brief 
	/// @param name 
	Entity::Entity(const std::string_view& name)
	: _name(name)
	{
		_instanceId = _nextInstanceId.fetch_add(1);
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
	const std::string& Entity::GetName() const
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
				_activeInHierarchy = _active;

				/* TODO
				for (std::shared_ptr<Component> component : _components)
				{
					component->RefreshEnabled();
				}

				for (const WeakEntity& child : _children)
				{
					std::shared_ptr<Entity> entity = child.Lock();
					entity->NotifyActivationChanged();
				}
				*/
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
	const std::vector<std::shared_ptr<Component>>& Entity::GetComponents() const
	{
		return _components;
	}

	/// @brief 
	/// @param descriptor 
	/// @return 
	std::shared_ptr<Component> Entity::GetComponent(const ReflectionDescriptor& descriptor)
	{
		for (const std::shared_ptr<Component>& component : _components)
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
	std::shared_ptr<Component> Entity::GetComponentInParent(const ReflectionDescriptor& descriptor)
	{
		std::shared_ptr<Component> component = GetComponent(descriptor);
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
	std::shared_ptr<Component> Entity::AddComponent(const ReflectionDescriptor& descriptor)
	{
		std::shared_ptr<Component> existingComponent = GetComponent(descriptor);
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}

		return AddComponent(std::static_pointer_cast<Component>(descriptor.CreateSharedInstance()));
	}

	/// @brief 
	/// @param component 
	/// @return 
	std::shared_ptr<Component> Entity::AddComponent(std::shared_ptr<Component> component)
	{
		_components.push_back(component);
		component->AttachTo(shared_from_this());

		//awakeAndStart = World::GetInstance()->GetEditorPlaying();
		
		if (_active == true)
		{
			component->OnAwake();
			component->OnStart();
		}

		return component;
	}

	/// @brief 
	/// @param component 
	void Entity::RemoveComponent(std::shared_ptr<Component> component)
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
			std::shared_ptr<Component> component = _components[index];
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
	const std::vector<WeakEntity>& Entity::GetChildren() const
	{
		return _children;
	}

	/// @brief 
	/// @return 
	uint32_t Entity::GetSiblingIndex() const
	{
		std::shared_ptr<Entity> parentLock = _parent.Lock();
		if (parentLock != nullptr)
		{
			for (uint32_t index = 0; index < parentLock->_children.size(); ++index)
			{
				if (parentLock->_children[index].Lock().get() == this)
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
		std::shared_ptr<Entity> parentLock = _parent.Lock();
		if (parentLock != nullptr)
		{
			uint32_t initialIndex = GetSiblingIndex();
			for (uint32_t i = initialIndex; i > index; --i)
			{
				parentLock->_children[i] = parentLock->_children[i - 1];
			}
			parentLock->_children[index] = WeakEntity(shared_from_this());
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

		std::shared_ptr<Entity> parentLock = _parent.Lock();
		if (parentLock != nullptr)
		{
			std::shared_ptr<Entity> thiz = shared_from_this();

			auto itEnd = parentLock->_children.end();
			for (auto it = parentLock->_children.begin(); it != itEnd; ++it)
			{
				if (it->Lock() == thiz)
				{
					parentLock->_children.erase(it);
					break;
				}
			}
			// todo assert
		}

		parentLock = parent.Lock();
		if (parentLock != nullptr)
		{
			bool exist = false;
			for (const WeakEntity& child : parentLock->_children)
			{
				if (child.Lock() == shared_from_this() || child.GetInstanceId() == _instanceId)
				{
					exist = true;
					break;
				}
			}
			if (exist == false)
			{
				parentLock->_children.emplace_back(shared_from_this());
			}
		}
		_parent = parent;
	}

	/// @brief 
	/// @return 
	const WeakEntity& Entity::GetParent() const
	{
		return _parent;
	}

	/// @brief 
	/// @return 
	Scene* Entity::GetScene() const
	{
		return _scene;
	}
}