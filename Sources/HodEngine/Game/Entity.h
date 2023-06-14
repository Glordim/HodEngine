#pragma once

#include <cstdint>
#include <string>

namespace hod::game
{
	class Entity final
	{
	public:

		using Id = uint64_t;

	public:

						Entity(const std::string_view& name);
						Entity(const Entity&) = default;
						Entity(Entity&&) = default;
						~Entity() = default;

		const Entity&	operator = (const Entity& copy);
		const Entity&	operator = (Entity&& move);

	public:

		Id					GetId() const;
		const std::string&	GetName() const;
		bool				GetActive() const;

	private:

		Id				_id;
		std::string		_name;
		bool			_active;
	};
}