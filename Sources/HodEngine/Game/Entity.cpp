#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

namespace hod::game
{
	Entity::Id Entity::_nextId = 0;

	/// @brief 
	/// @param name 
	Entity::Entity(const std::string_view& name)
	: _name(name)
	{
		++_nextId;
		_id = _nextId;
	}

	/// @brief 
	/// @param other 
	std::shared_ptr<Entity> Entity::Clone()
	{
		std::shared_ptr<Entity> clone = std::make_shared<Entity>(_name);

		clone->_components.reserve(_components.size());
		for (std::shared_ptr<Component> component : _components)
		{
			ReflectionDescriptor* reflectionDescriptor = component->GetReflectionDescriptorV();
			std::shared_ptr<Component> cloneComponent = std::static_pointer_cast<Component>(reflectionDescriptor->CreateSharedInstance());

			reflectionDescriptor->Copy(component.get(), cloneComponent.get());

			cloneComponent->SetEntity(clone);
			cloneComponent->Construct();

			clone->_components.push_back(cloneComponent);
		}

		return clone;
	}

	/// @brief 
	/// @return 
	Entity::Id Entity::GetId() const
	{
		return _id;
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
				Start();
			}
		 }
	 }
	 
	 void Entity::Awake()
	 {
		#if defined(HOD_EDITOR)
		if (World::GetInstance()->GetEditorPlaying() == false)
		{
			return;
		}
		#endif
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
		#if defined(HOD_EDITOR)
		if (World::GetInstance()->GetEditorPlaying() == false)
		{
			return;
		}
		#endif
		if (_started == false)
		{
			_started = true;

			for (std::weak_ptr<Component> component : _components)
			{
				component.lock()->OnStart();
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
	std::weak_ptr<Component> Entity::GetComponent(MetaType metaType)
	{
		for (const std::shared_ptr<Component>& component : _components)
		{
			if (component->GetMetaType() == metaType)
			{
				return component;
			}
		}
		return std::weak_ptr<Component>();
	}

	/// @brief 
	/// @param descriptor 
	/// @return 
	std::weak_ptr<Component> Entity::AddComponent(const ReflectionDescriptor& descriptor, bool awakeAndStart)
	{
		std::shared_ptr<Component> existingComponent = GetComponent(descriptor.GetMetaType()).lock();
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}

		return AddComponent(std::static_pointer_cast<Component>(descriptor.CreateSharedInstance()), awakeAndStart);
	}

	/// @brief 
	/// @param component 
	/// @return 
	std::weak_ptr<Component> Entity::AddComponent(std::shared_ptr<Component> component, bool awakeAndStart)
	{
		_components.push_back(component);
		component->SetEntity(weak_from_this());

		_onAddComponentEvent.Emit(component);

#if defined(HOD_EDITOR)
		awakeAndStart = World::GetInstance()->GetEditorPlaying();
#endif
		if (_active == true && awakeAndStart == true)
		{
			component->OnAwake();
			component->OnStart();
		}

		return component;
	}

	/// @brief 
	/// @param component 
	void Entity::RemoveComponent(std::weak_ptr<Component> component)
	{
		auto it = std::find(_components.begin(), _components.end(), component.lock());
		if (it != _components.end())
		{
			_components.erase(it);
		}
	}
}