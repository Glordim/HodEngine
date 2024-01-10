#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/Type.hpp"

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::game
{
	class Component;

	class Entity final : public std::enable_shared_from_this<Entity>
	{
	public:

		using Id = uint64_t;

	public:

						Entity(const std::string_view& name);
						Entity(const Entity&) = delete;
						Entity(Entity&&) = delete;
						~Entity() = default;

		const Entity&	operator = (const Entity&) = delete;
		const Entity&	operator = (Entity&&) = delete;

	public:

		Id					GetId() const;
		const std::string&	GetName() const;
		void				SetName(const std::string_view& name);
		bool				GetActive() const;
		void				SetActive(bool active);

		void				Awake();
		void				Start();

		std::vector<std::weak_ptr<game::Component>>	GetComponents() const;

		template<typename _Component_>
		std::weak_ptr<_Component_>	GetComponent();
		std::weak_ptr<Component>	GetComponent(MetaType metaType);

		template<typename _Component_>
		std::weak_ptr<_Component_>	AddComponent(bool awakeAndStart = true);
		std::weak_ptr<Component>	AddComponent(const ReflectionDescriptor& descriptor, bool awakeAndStart = true);

		template<typename _Component_>
		void						RemoveComponent();
		void						RemoveComponent(std::weak_ptr<Component> component);

		Event<std::weak_ptr<Component>>&	GetAddComponentEvent() { return _onAddComponentEvent; }
		Event<std::weak_ptr<Component>>&	GetRemoveComponentEvent() { return _onRemoveComponentEvent; }

	private:

		std::weak_ptr<Component>	AddComponent(std::shared_ptr<Component> instance, bool awakeAndStart);

	private:

		Id				_id;
		std::string		_name;
		bool			_active = false;
		bool			_awaked = false;
		bool			_started = false;

		std::vector<std::shared_ptr<Component>>	_components; // map ?

		Event<std::weak_ptr<Component>>	_onAddComponentEvent;
		Event<std::weak_ptr<Component>>	_onRemoveComponentEvent;
	};
}

#include "Entity.inl"
