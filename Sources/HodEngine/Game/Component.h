#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.h>
#include <HodEngine/Core/Reflection/ReflectionHelper.h>
#include <memory>
#include <functional>

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
		class Entity;

		///@brief 
		class Component : public Object, public std::enable_shared_from_this<Component>
		{
			REFLECTED_CLASS(Component, Object)

		public:

			void							SetEntity(const std::weak_ptr<Entity>& entity);
			const std::weak_ptr<Entity>&	GetEntity() const;
			virtual const char*				GetType() const = 0;

			virtual void		PushToRenderQueue(renderer::RenderQueue& renderQueue);

		protected:

								Component() = default;
								Component(const Component&) = delete;
								Component(Component&&) = delete;
			virtual				~Component() = default;

			void				operator=(const Component&) = delete;
			void				operator=(Component&&) = delete;

		private:

			std::weak_ptr<Entity>	_entity;
		};
	}
}

#include "Component.inl"
