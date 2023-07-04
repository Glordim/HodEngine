#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.h>
#include <HodEngine/Core/Reflection/ReflectionHelper.h>
#include <memory>
#include <functional>

#include "HodEngine/Core/Type.h"
#include "HodEngine/Core/Object.h"
#include "HodEngine/Game/src/ComponentDescriptor.h"

namespace hod
{
	namespace renderer
	{
		class RenderQueue;
	}

	namespace game
	{
		class Entity;
		class ComponentDescriptor;

		///@brief 
		class Component : public Object, public std::enable_shared_from_this<Component>
		{
			REFLECTED_ABSTRACT_DERIVED_CLASS(Component, Object)
			BASE_META_TYPE(Component)

		public:

			virtual const ComponentDescriptor& GetDescriptor() = 0;	// todo base macro ?

		public:

			const std::weak_ptr<Entity>&	GetEntity() const;
			virtual const char*				GetType() const = 0;

			virtual void		PushToRenderQueue(renderer::RenderQueue& renderQueue);

		protected:

								Component(const std::weak_ptr<Entity>& entity);
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
