#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <atomic>
#include <cstdint>
#include <memory>

#include "HodEngine/Core/Event.hpp"
#include "HodEngine/Core/TypeTrait.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Weakable/Weakable.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include "HodEngine/Game/WeakEntity.hpp"

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline game
{
	class Component;
	class PrefabResource;
	class Scene;

	class HOD_GAME_API Entity final : public Weakable
	{
		REFLECTED_CLASS_NO_VIRTUAL(Entity);
		friend class Scene;
		friend class SceneSerializer;

	public:
		enum class InternalState : uint8_t
		{
			None,
			Constructed,
			Awaked,
			Started,
			Destructed,
		};
		REFLECTED_ENUM(HOD_GAME_API, InternalState);

		using ChildrenChangedEvent = core::Event<>;

	public:
		Entity(const std::string_view& name);
		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		~Entity();

		const Entity& operator=(const Entity&) = delete;
		const Entity& operator=(Entity&&) = delete;

	public:
		void          SetName(const std::string_view& name);
		const String& GetName() const;

		void SetActive(bool active);
		bool GetActive() const;
		bool IsActiveInHierarchy() const;

		// Hierarchy
		void              SetParent(const WeakEntity& parent);
		const WeakEntity& GetParent() const;

		const Vector<WeakEntity>& GetChildren() const;

		void     SetSiblingIndex(uint32_t index);
		uint32_t GetSiblingIndex() const;

		ChildrenChangedEvent& GetChildrenChangedEvent();

		// Components
		const Vector<Component*>& GetComponents() const;

		template<typename _Component_>
		_Component_* GetComponent();
		Component*   GetComponent(const ReflectionDescriptor& descriptor);

		template<typename _Component_>
		_Component_* GetComponentInParent();
		Component*   GetComponentInParent(const ReflectionDescriptor& descriptor);

		/// @brief Returns the one already there if any. Otherwise adds first the components it requires that are missing
		/// (ReflectionTraitRequireComponent, recursively, see GetMissingRequiredComponents), then it.
		template<typename _Component_>
		_Component_* AddComponent();
		Component*   AddComponent(const ReflectionDescriptor& descriptor);

		/// @brief Refused (an error is logged, returns false) while another component of the entity requires it, see GetComponentRequiring
		template<typename _Component_>
		bool RemoveComponent();
		bool RemoveComponent(const ReflectionDescriptor& descriptor);
		/// @brief Deletes it. Refused (an error is logged, returns false) while another component of the entity requires it, see GetComponentRequiring
		bool RemoveComponent(Component* component);

		/// @brief What AddComponent(descriptor) would add before it: the components it requires (and that they require) missing from the entity,
		/// dependencies first. For a component already on the entity: what it requires and is missing (data written without it).
		Vector<const ReflectionDescriptor*> GetMissingRequiredComponents(const ReflectionDescriptor& descriptor) const;
		/// @brief Another component of the entity requiring this one, that no other component of the entity can stand in for (nullptr when none)
		Component* GetComponentRequiring(const Component* component) const;

		// Serialization
		void                            SetPrefabResource(std::shared_ptr<PrefabResource> prefabResource); // TODO move in private ?
		std::shared_ptr<PrefabResource> GetPrefabResource() const;

		void   SetScene(Scene* scene);
		Scene* GetScene() const;

		uint64_t GetInstanceId() const;
		uint64_t GetLocalId() const;
		void     SetLocalId(uint64_t localId); // TODO move in private ?

		InternalState GetInternalState() const;

	private:
		Component* AddComponent(Component* instance);
		/// @brief For loading (SceneSerializer): the data stays as written, its requirements are not added
		Component* AddComponentWithoutRequirements(const ReflectionDescriptor& descriptor);
		void       CollectMissingRequiredComponents(const ReflectionDescriptor& descriptor, Vector<const ReflectionDescriptor*>& missing,
		                                            Vector<const ReflectionDescriptor*>& visiting) const;
		const Component* FindComponent(const ReflectionDescriptor& descriptor) const;

		void NotifyActivationChanged();

		void EnableComponents();
		void DisableComponents();

		void ProcessActivation();

		void Construct();
		void Awake();
		void Enable();
		void Start();
		void Disable();
		void Destruct();

	private:
		String _name;

		bool          _active = true;
		bool          _activeInHierarchy = false;
		InternalState _internalState = InternalState::None;

		Vector<WeakEntity>   _children;
		WeakEntity           _parent;
		ChildrenChangedEvent _childrenChangedEvent;

		Vector<Component*> _components;

		Scene*                          _scene = nullptr; // TODO never set !!!
		std::shared_ptr<PrefabResource> _prefabResource = nullptr;

		uint64_t _instanceId = 0;
		uint64_t _localId = 0;

	private:
		static std::atomic<uint64_t> _nextInstanceId;
	};
}

#include "Entity.inl"
