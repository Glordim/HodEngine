#include "HodEngine/Game/Entity.h"

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
}