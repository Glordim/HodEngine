#pragma once
#include "HodEngine/Game/Export.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

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
	class Scene;

	class HOD_GAME_API Entity final : public std::enable_shared_from_this<Entity>
	{
		REFLECTED_CLASS_NO_VIRTUAL(Entity);
		friend class Scene;

	public:

		enum class InternalState : uint8_t
		{
			None,
			Constructed,
			Awaked,
			Started,
			Destructed,
		};

	public:

														Entity(const std::string_view& name);
														Entity(const Entity&) = delete;
														Entity(Entity&&) = delete;
														~Entity() = default;

		const Entity&									operator = (const Entity&) = delete;
		const Entity&									operator = (Entity&&) = delete;

	public:

		void											SetName(const std::string_view& name);
		const std::string&								GetName() const;

		void											SetActive(bool active);
		bool											GetActive() const;
		bool											IsActiveInHierarchy() const;

		// Hierarchy
		void											SetParent(const WeakEntity& parent);
		const WeakEntity&								GetParent() const;

		const std::vector<WeakEntity>&					GetChildren() const;

		void											SetSiblingIndex(uint32_t index);
		uint32_t										GetSiblingIndex() const;

		// Components
		const std::vector<std::shared_ptr<Component>>&	GetComponents() const;

		template<typename _Component_>
		std::shared_ptr<_Component_>					GetComponent();
		std::shared_ptr<Component>						GetComponent(const ReflectionDescriptor& descriptor);

		template<typename _Component_>
		std::shared_ptr<_Component_>					GetComponentInParent();
		std::shared_ptr<Component>						GetComponentInParent(const ReflectionDescriptor& descriptor);

		template<typename _Component_>
		std::shared_ptr<_Component_>					AddComponent();
		std::shared_ptr<Component>						AddComponent(const ReflectionDescriptor& descriptor);

		template<typename _Component_>
		void											RemoveComponent();
		void											RemoveComponent(const ReflectionDescriptor& descriptor);
		void											RemoveComponent(std::shared_ptr<Component> component);

		// Serialization
		void											SetPrefabResource(std::shared_ptr<PrefabResource> prefabResource); // TODO move in private ?
		std::shared_ptr<PrefabResource>					GetPrefabResource() const;

		void											SetScene(Scene* scene);
		Scene*											GetScene() const;

		uint64_t										GetInstanceId() const;
		uint64_t										GetLocalId() const;
		void											SetLocalId(uint64_t localId); // TODO move in private ?

		InternalState									GetInternalState() const;

	private:

		std::shared_ptr<Component>						AddComponent(std::shared_ptr<Component> instance);

		void											NotifyActivationChanged();

		void											EnableComponents();
		void											DisableComponents();

		void											ProcessActivation();

		void											Construct();
		void											Awake();
		void											Enable();
		void											Start();
		void											Disable();
		void											Destruct();

	private:

		std::string										_name;

		bool											_active = false;
		bool											_activeInHierarchy = false;
		InternalState									_internalState = InternalState::None;

		std::vector<WeakEntity>							_children;
		WeakEntity										_parent;

		std::vector<std::shared_ptr<Component>>			_components;
		
		Scene*											_scene = nullptr; // TODO never set !!!
		std::shared_ptr<PrefabResource>					_prefabResource = nullptr;

		uint64_t										_instanceId = 0;
		uint64_t										_localId = 0;

	private:

		static std::atomic<uint64_t>					_nextInstanceId;
	};
}

#include "Entity.inl"
