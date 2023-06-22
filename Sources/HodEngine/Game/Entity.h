#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace hod::game
{
	class Component;

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
		void				SetName(const std::string_view& name);
		bool				GetActive() const;

		template<typename _Component_>
		std::weak_ptr<_Component_>	GetComponent();

		template<typename _Component_>
		std::weak_ptr<_Component_>	AddComponent();

	private:

		Id				_id;
		std::string		_name;
		bool			_active;

		std::vector<std::shared_ptr<Component>>	_components; // map ?
	};
}

#include "Entity.inl"
