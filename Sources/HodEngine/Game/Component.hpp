#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <memory>
#include <atomic>

namespace hod::game
{
	class Entity;
	class Scene;

	///@brief 
	class HOD_GAME_API Component : public std::enable_shared_from_this<Component>
	{
		REFLECTED_CLASS_NO_PARENT(Component)

		friend class Scene;
		friend class Entity;

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

		uint64_t				GetInstanceId() const;

		uint64_t				GetLocalId() const;
		void					SetLocalId(uint64_t localId);

		std::shared_ptr<Entity>	GetOwner() const;

		void					SetEnabled(bool enabled);
		bool					GetEnabled() const;
		bool					IsEnabledInHierarchy() const;

		virtual void			OnConstruct();
		virtual void			OnAwake();
		virtual void			OnEnable();
		virtual void			OnStart();
		virtual void			OnUpdate();
		virtual void			OnDisable();
		virtual void			OnDestruct();

		// TODO Remove
		void					SetEntity(const std::shared_ptr<Entity>& entity);
		std::shared_ptr<Entity>	GetEntity() const;
		//

	protected:

								Component();
								Component(const Component&) = delete;
								Component(Component&&) = delete;
		virtual					~Component() = default;

		void					operator=(const Component&) = delete;
		void					operator=(Component&&) = delete;

	private:

		void					AttachTo(const std::shared_ptr<Entity>& entity);

		InternalState			GetInternalState() const;

		void					Construct();
		void					Awake();
		void					Enable();
		void					Start();
		void					Disable();
		void					Destruct();

	private:

		uint64_t				_instanceId = 0;
		uint64_t				_localId = 0;

		std::weak_ptr<Entity>	_entity;

		InternalState			_internalState = InternalState::None;

		bool					_enabled = true;
		bool					_enabledInHierarchy = false;

	private:

		static std::atomic<uint64_t> _nextInstanceId;
	};
}
