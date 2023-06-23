#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.h>
#include <memory>

#include "HodEngine/Core/Type.h"
#include "HodEngine/Core/Object.h"

namespace hod
{
	namespace renderer
	{
		class RenderQueue;
	}

	namespace game
	{
		class Actor;
		class Entity;

		///@brief 
		class Component : public Object, public std::enable_shared_from_this<Component>
		{
			REFLECTED_ABSTRACT_DERIVED_CLASS(Component, Object)
			BASE_META_TYPE(Component)

			friend class Actor;

		public:

			const std::weak_ptr<Entity>&	GetEntity() const;
			Actor*				GetActor() const;
			virtual const char*	GetType() const = 0;

			virtual void		PushToRenderQueue(renderer::RenderQueue& renderQueue);

		protected:

								Component(Actor* actor); // todo remove
								Component(const std::weak_ptr<Entity>& entity);
								Component(const Component&) = delete;
								Component(Component&&) = delete;
			virtual				~Component() = default;

			void				operator=(const Component&) = delete;
			void				operator=(Component&&) = delete;

		private:

			Actor*				_actor = nullptr;
			std::weak_ptr<Entity>	_entity;
		};
	}
}

#include "Component.inl"
