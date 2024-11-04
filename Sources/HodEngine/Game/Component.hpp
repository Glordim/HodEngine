#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>
#include <memory>
#include <functional>

#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Core/UID.hpp"

namespace hod
{
	struct Color;

	namespace renderer
	{
		class RenderQueue;
	}

	namespace game
	{
		class Entity;
		class Scene;

		///@brief 
		class HOD_GAME_API Component : public std::enable_shared_from_this<Component>
		{
			REFLECTED_CLASS_NO_PARENT(Component, HOD_GAME_API)

			friend class Scene;
			friend class Entity;

		public:

			void					SetEntity(const std::shared_ptr<Entity>& entity);
			std::shared_ptr<Entity>	GetEntity() const;

			void					SetEnable(bool enable);
			bool					GetEnable() const;

			const UID&			GetUid() const { return _uid; }
			const UID&			GetLocalId() const { return _localId; }
			void				SetLocalId(const UID& uid) { _localId = uid; }

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

			UID						_uid;
			UID						_localId;
			std::weak_ptr<Entity>	_entity;

			bool					_enable = true;
			bool					_wasEnable = false;
		};
	}
}

#include "Component.inl"
