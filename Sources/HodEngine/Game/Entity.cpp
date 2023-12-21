#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Component.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"


namespace hod::game
{
	/// @brief 
	/// @param name 
	Entity::Entity(const std::string_view& name)
	: _name(name)
	{
		static Entity::Id nextId = 0;
		++nextId;

		_id = nextId;
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
		_active = active;
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
	std::weak_ptr<Component> Entity::AddComponent(const ReflectionDescriptor& descriptor)
	{
		std::shared_ptr<Component> existingComponent = GetComponent(descriptor.GetMetaType()).lock();
		if (existingComponent != nullptr)
		{
			return existingComponent;
		}

		return AddComponent(std::static_pointer_cast<Component>(descriptor.CreateSharedInstance()));
	}

	/// @brief 
	/// @param component 
	/// @return 
	std::weak_ptr<Component> Entity::AddComponent(std::shared_ptr<Component> component)
	{
		_components.push_back(component);
		component->SetEntity(weak_from_this());

		_onAddComponentEvent.Emit(component);

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