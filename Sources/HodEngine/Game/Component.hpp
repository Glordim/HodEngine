#pragma once

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
		class Component : public std::enable_shared_from_this<Component>
		{
			REFLECTED_CLASS_NO_PARENT(Component)

			friend class Scene;
			friend class Entity;

		public:

			void							SetEntity(const std::weak_ptr<Entity>& entity);
			const std::weak_ptr<Entity>&	GetEntity() const;

			virtual void		PushToRenderQueue(renderer::RenderQueue& renderQueue) {}; // TODO move it in RendererComponent ?
			virtual void		PushPickingToRenderQueue(renderer::RenderQueue& renderQueue, const Color& colorId) {}; // TODO move it in RendererComponent ?

			const UID&			GetUid() const { return _uid; }

			virtual void		OnAwake() {};
			virtual void		OnStart() {};
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
			std::weak_ptr<Entity>	_entity;
		};
	}
}

#include "Component.inl"
