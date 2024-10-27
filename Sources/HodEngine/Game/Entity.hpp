#pragma once
#include "HodEngine/Game/Export.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Core/UID.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::game
{
	class Component;
	class PrefabResource;

	class HOD_GAME_API Entity final : public std::enable_shared_from_this<Entity>
	{
		REFLECTED_CLASS_NO_VIRTUAL(Entity, HOD_GAME_API);

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

		UID					GetLocalId() const { return _localId; }

		Id					GetId() const;
		const std::string&	GetName() const;
		void				SetName(const std::string_view& name);
		bool				GetActive() const;
		void				SetActive(bool active);

		std::shared_ptr<Entity>	Clone();

		void				Awake();
		void				Start();

		std::vector<std::weak_ptr<game::Component>>	GetComponents() const;

		template<typename _Component_>
		std::shared_ptr<_Component_>	GetComponent();
		std::shared_ptr<Component>		GetComponent(MetaType metaType);

		template<typename _Component_>
		std::shared_ptr<_Component_>	AddComponent(bool awakeAndStart = true);
		std::shared_ptr<Component>		AddComponent(const ReflectionDescriptor& descriptor, bool awakeAndStart = true);

		template<typename _Component_>
		void						RemoveComponent();
		void						RemoveComponent(std::shared_ptr<Component> component);

		Event<std::shared_ptr<Component>>&	GetAddComponentEvent() { return _onAddComponentEvent; }
		Event<std::shared_ptr<Component>>&	GetRemoveComponentEvent() { return _onRemoveComponentEvent; }

		void							SetPrefabResource(std::shared_ptr<PrefabResource> prefabResource);
		std::shared_ptr<PrefabResource>	GetPrefabResource() const;

	private:

		std::shared_ptr<Component>	AddComponent(std::shared_ptr<Component> instance, bool awakeAndStart);

	private:

		static Entity::Id	_nextId; // todo std::atomic ?

	private:

		UID				_localId;

		Id				_id;
		std::string		_name;
		bool			_active = false;
		bool			_awaked = false;
		bool			_started = false;

		std::shared_ptr<PrefabResource> _prefabResource = nullptr;

		std::vector<std::shared_ptr<Component>>	_components; // map ?

		Event<std::shared_ptr<Component>>	_onAddComponentEvent;
		Event<std::shared_ptr<Component>>	_onRemoveComponentEvent;
	};
}

#include "Entity.inl"
