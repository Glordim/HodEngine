#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Weakable/Weakable.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include "HodEngine/Game/Entity.hpp"
#include <memory>
#include <atomic>

namespace hod::inline game
{
	class Entity;
	class Scene;
	class World;

	///@brief 
	class HOD_GAME_API Component : public Weakable
	{
		REFLECTED_CLASS_NO_PARENT(Component)

		friend class Scene;
		friend class Entity;
		friend class hod::Allocator;

	public:

		Entity*					GetOwner() const;
		Scene*					GetScene() const;
		World*					GetWorld() const;

		// The components of its entity (see Entity): null without an owner
		template<typename _Component_>
		_Component_*			GetComponent() const;
		Component*				GetComponent(const ReflectionDescriptor& descriptor) const;

		// Its entity's first, then up the hierarchy
		template<typename _Component_>
		_Component_*			GetComponentInParent() const;
		Component*				GetComponentInParent(const ReflectionDescriptor& descriptor) const;

		void					SetEnabled(bool enabled);
		bool					GetEnabled() const;
		bool					IsEnabledInHierarchy() const;

		virtual void			OnConstruct();
		virtual void			OnAwake();
		virtual void			OnEnable();
		virtual void			OnStart();
		/// @brief Each frame while enabled, deltaTime scaled (0 while the time scale is: see World::GetTime)
		virtual void			OnUpdate(float deltaTime);
		/// @brief Each fixed step while enabled (WorldTime::GetFixedTimestep), none while the time scale is 0
		virtual void			OnFixedUpdate();
		virtual void			OnDisable();
		virtual void			OnDestruct();

		uint64_t				GetInstanceId() const;
		uint64_t				GetLocalId() const;
		void					SetLocalId(uint64_t localId); // TODO move in private ?

		enum class InternalState : uint8_t
		{
			None,
			Constructed,
			Awaked,
			Started,
			Destructed,
		};

		InternalState			GetInternalState() const;

	protected:

								Component();
								Component(const Component&) = delete;
								Component(Component&&) = delete;
		virtual					~Component() = default;

		void					operator=(const Component&) = delete;
		void					operator=(Component&&) = delete;

	private:

		void					AttachTo(Entity* owner);

		void					Construct();
		void					Awake();
		void					Enable();
		void					Start();
		void					Disable();
		void					Destruct();

	private:

		WeakPtr<Entity>			_owner;

		InternalState			_internalState = InternalState::None;

		bool					_enabled = true;
		bool					_enabledInHierarchy = false;

		uint64_t				_instanceId = 0;
		uint64_t				_localId = 0;

	private:

		static std::atomic<uint64_t> _nextInstanceId;
	};

	/// @brief
	/// @tparam _Component_
	/// @return
	template<typename _Component_>
	_Component_* Component::GetComponent() const
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponent()");
		Entity* owner = GetOwner();
		return (owner != nullptr) ? owner->GetComponent<_Component_>() : nullptr;
	}

	/// @brief
	/// @tparam _Component_
	/// @return
	template<typename _Component_>
	_Component_* Component::GetComponentInParent() const
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use GetComponentInParent()");
		Entity* owner = GetOwner();
		return (owner != nullptr) ? owner->GetComponentInParent<_Component_>() : nullptr;
	}
}
