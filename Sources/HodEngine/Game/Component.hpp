#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>
#include <memory>
#include <functional>

#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Core/UID.hpp"

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

		uint64_t				GetInstanceId() const { return _instanceId; }
		uint64_t				GetLocalId() const { return _localId; }
		void					SetLocalId(uint64_t localId) { _localId = localId; }

		void					SetEntity(const std::shared_ptr<Entity>& entity);
		std::shared_ptr<Entity>	GetEntity() const;

		void					SetEnable(bool enable);
		bool					GetEnable() const;

		virtual void		OnAwake() {};
		virtual void		OnStart() {};
		virtual void		OnEnable() {};
		virtual void		OnDisable() {};
		virtual void		OnUpdate() {};

	protected:

							Component();
							Component(const Component&) = delete;
							Component(Component&&) = delete;
		virtual				~Component() = default;

		void				operator=(const Component&) = delete;
		void				operator=(Component&&) = delete;

		virtual void		OnConstruct() {};

	private:

		void				Construct() { OnConstruct(); };

	private:

		static uint64_t			_nextInstanceId; // todo std::atomic ?

	private:

		uint64_t				_instanceId = 0;
		uint64_t				_localId = 0;

		std::weak_ptr<Entity>	_entity;

		bool					_enable = true;
		bool					_wasEnable = false;
	};
}
