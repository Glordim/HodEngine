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
	uint64_t Entity::_nextInstanceId = 0;

	DESCRIBE_REFLECTED_CLASS(Entity, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Component::_instanceId, "_instanceId")->AddTrait<ReflectionTraitNoSerialization>();
		AddPropertyT(reflectionDescriptor, &Entity::_localId, "_localId");//->AddTrait<ReflectionTraitHide>();

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
		++_nextInstanceId;
		_instanceId = _nextInstanceId;
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
	/// @return 
	bool Entity::GetActive() const
	{
		return _active;
	}

	/// @brief 
	/// @param active 
	void Entity::SetActive(bool active)
	{
		if (_active != active)
		{
			_active = active;
			if (active == true)
			{
				Awake();
				OnEnable();
				Start();
			}
		 }
	 }
	 
	 void Entity::Awake()
	 {
		if (World::GetInstance()->GetEditorPlaying() == false)
		{
			return;
		}

		if (_awaked == false)
		{
			_awaked = true;

			for (std::weak_ptr<Component> component : _components)
			{
				component.lock()->OnAwake();
			}
		}
	 }

	 void Entity::Start()
	 {
		if (World::GetInstance()->GetEditorPlaying() == false)
		{
			return;
		}

		if (_started == false)
		{
			_started = true;

			for (std::weak_ptr<Component> component : _components)
			{
				component.lock()->OnStart();
			}
		}
	}

	void Entity::OnEnable()
	 {
		if (World::GetInstance()->GetEditorPlaying() == false)
		{
			return;
		}

		if (_started == false)
		{
			_started = true;

			for (std::weak_ptr<Component> component : _components)
			{
				component.lock()->OnEnable();
			}
		}
	}

	void Entity::OnDisable()
	 {
		if (World::GetInstance()->GetEditorPlaying() == false)
		{
			return;
		}

		if (_started == false)
		{
			_started = true;

			for (std::weak_ptr<Component> component : _components)
			{
				component.lock()->OnDisable();
			}
		}
	}

	/// @brief 
	/// @return 
	std::vector<std::weak_ptr<game::Component>> Entity::GetComponents() const
	{
		std::vector<std::weak_ptr<game::Component>> components;
		components.reserve(_components.size());
 
		for (std::weak_ptr<game::Component> component : _components)
		{
			components.push_back(component);
		}

		return components;
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
	std::shared_ptr<Component> Entity::AddComponent(const ReflectionDescriptor& descriptor, bool awakeAndStart)
	{
		std::shared_ptr<Component> existingComponent = GetComponent(descriptor);
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}

		return AddComponent(std::static_pointer_cast<Component>(descriptor.CreateSharedInstance()), awakeAndStart);
	}

	/// @brief 
	/// @param component 
	/// @return 
	std::shared_ptr<Component> Entity::AddComponent(std::shared_ptr<Component> component, bool awakeAndStart)
	{
		_components.push_back(component);
		component->SetEntity(shared_from_this());

		_onAddComponentEvent.Emit(component);

		awakeAndStart = World::GetInstance()->GetEditorPlaying();
		
		if (_active == true && awakeAndStart == true)
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
	uint32_t Entity::GetChildCount() const
	{
		return static_cast<uint32_t>(_children.size());
	}

	/// @brief 
	/// @param index 
	/// @return 
	const WeakEntity& Entity::GetChild(uint32_t index)
	{
		return _children[index];
	}

	/// @brief 
	/// @return 
	uint32_t Entity::GetSiblingIndex() const
	{
		std::shared_ptr<Entity> parentLock = _parent.Lock();
		if (parentLock != nullptr)
		{
			for (uint32_t index = 0; index < parentLock->GetChildCount(); ++index)
			{
				if (parentLock->GetChild(index).Lock().get() == this)
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
}