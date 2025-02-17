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

#include "HodEngine/Game/WeakEntity.hpp"

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
		REFLECTED_CLASS_NO_VIRTUAL(Entity);

	public:

						Entity(const std::string_view& name);
						Entity(const Entity&) = delete;
						Entity(Entity&&) = delete;
						~Entity() = default;

		const Entity&	operator = (const Entity&) = delete;
		const Entity&	operator = (Entity&&) = delete;

	public:

		uint32_t							GetChildCount() const;
		const WeakEntity&					GetChild(uint32_t index);

		uint32_t							GetSiblingIndex() const;
		void								SetSiblingIndex(uint32_t index);

		const WeakEntity&					GetParent() const;
		void								SetParent(const WeakEntity& parent);

		uint64_t				GetInstanceId() const { return _instanceId; }
		uint64_t				GetLocalId() const { return _localId; }
		void					SetLocalId(uint64_t localId) { _localId = localId; }

		const std::string&	GetName() const;
		void				SetName(const std::string_view& name);
		bool				GetActiveSelf() const;
		void				SetActiveSelf(bool activeSelf);
		bool				IsActive() const;

		void				Awake();
		void				Start();
		void				OnEnable();
		void				OnDisable();

		std::vector<std::weak_ptr<game::Component>>	GetComponents() const;

		template<typename _Component_>
		std::shared_ptr<_Component_>	GetComponent();
		std::shared_ptr<Component>		GetComponent(const ReflectionDescriptor& descriptor);

		template<typename _Component_>
		std::shared_ptr<_Component_>	GetComponentInParent();
		std::shared_ptr<Component>		GetComponentInParent(const ReflectionDescriptor& descriptor);

		template<typename _Component_>
		std::shared_ptr<_Component_>	AddComponent(bool awakeAndStart = true);
		std::shared_ptr<Component>		AddComponent(const ReflectionDescriptor& descriptor, bool awakeAndStart = true);

		template<typename _Component_>
		void						RemoveComponent();
		void						RemoveComponent(const ReflectionDescriptor& descriptor);
		void						RemoveComponent(std::shared_ptr<Component> component);

		Event<std::shared_ptr<Component>>&	GetAddComponentEvent() { return _onAddComponentEvent; }
		Event<std::shared_ptr<Component>>&	GetRemoveComponentEvent() { return _onRemoveComponentEvent; }

		void							SetPrefabResource(std::shared_ptr<PrefabResource> prefabResource);
		std::shared_ptr<PrefabResource>	GetPrefabResource() const;

	private:

		std::shared_ptr<Component>	AddComponent(std::shared_ptr<Component> instance, bool awakeAndStart);

		void						NotifyActivationChanged();

		void						EnableComponents();
		void						DisableComponents();

	private:

		static uint64_t	_nextInstanceId; // todo std::atomic ?

		enum class State : uint8_t
		{
			Created,
			Awaked,
			Started,
			Destroyed,
		};

	private:

		uint64_t		_instanceId = 0;
		uint64_t		_localId = 0;

		std::string		_name;
		bool			_activeSelf = false;
		bool			_active = false;

		State			_state = State::Created;

		std::shared_ptr<PrefabResource> _prefabResource = nullptr;

		std::vector<std::shared_ptr<Component>>	_components; // map ?

		Event<std::shared_ptr<Component>>	_onAddComponentEvent;
		Event<std::shared_ptr<Component>>	_onRemoveComponentEvent;

		std::vector<WeakEntity>				_children;
		WeakEntity							_parent;
	};
}

#include "Entity.inl"
