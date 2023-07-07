#include "HodEngine/Game/Entity.h"
#include "HodEngine/Game/Component.h"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.h"


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
	/// @param copy 
	/// @return 
	const Entity& Entity::operator = (const Entity& copy)
	{
		_id = copy._id;
		_name = copy._name;
		_active = copy._active;

		return *this;
	}

	/// @brief 
	/// @param move 
	/// @return 
	const Entity& Entity::operator = (Entity&& move)
	{
		_id = move._id;
		_name = move._name;
		_active = move._active;

		move._id = 0;
		move._name.clear();
		move._active = false;

		return *this;
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
	/// @return 
	std::vector<std::weak_ptr<game::Component>> Entity::GetComponents() const
	{
		std::vector<std::weak_ptr<game::Component>> components(_components.size());
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
}