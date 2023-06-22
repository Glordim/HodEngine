#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.h>

namespace hod
{
	namespace renderer
	{
		class RenderQueue;
	}

	namespace game
	{
		class Actor;

		///@brief 
		class Component
		{
			REFLECTED_ABSTRACT_CLASS(Component)

			friend class Actor;

		public:

			Actor*				GetActor() const;
			virtual const char*	GetType() const = 0;

			virtual void		PushToRenderQueue(renderer::RenderQueue& renderQueue);

		protected:

								Component(Actor* actor);
								Component(const Component&) = delete;
								Component(Component&&) = delete;
			virtual				~Component() = default;

			void				operator=(const Component&) = delete;
			void				operator=(Component&&) = delete;

		private:

			Actor*				_actor = nullptr;
		};
	}
}

#include "Component.inl"
