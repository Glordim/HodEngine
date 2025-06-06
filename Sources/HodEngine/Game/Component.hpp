#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Weakable/Weakable.hpp>
#include <HodEngine/Core/Weakable/WeakPtr.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <memory>
#include <atomic>

namespace hod::game
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
		friend class Allocator;

	public:

		Entity*					GetOwner() const;
		Scene*					GetScene() const;
		World*					GetWorld() const;

		void					SetEnabled(bool enabled);
		bool					GetEnabled() const;
		bool					IsEnabledInHierarchy() const;

		virtual void			OnConstruct();
		virtual void			OnAwake();
		virtual void			OnEnable();
		virtual void			OnStart();
		virtual void			OnUpdate(float deltaTime);
		virtual void			OnFixedUpdate();
		virtual void			OnDisable();
		virtual void			OnDestruct();

		uint64_t				GetInstanceId() const;
		uint64_t				GetLocalId() const;
		void					SetLocalId(uint64_t localId); // TODO move in private ?

	protected:

								Component();
								Component(const Component&) = delete;
								Component(Component&&) = delete;
		virtual					~Component() = default;

		void					operator=(const Component&) = delete;
		void					operator=(Component&&) = delete;

	private:

		enum class InternalState : uint8_t
		{
			None,
			Constructed,
			Awaked,
			Started,
			Destructed,
		};

	private:

		void					AttachTo(Entity* owner);

		InternalState			GetInternalState() const;

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
}
