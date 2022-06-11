#pragma once

#include <HodEngine/Core/Src/Reflection/ReflectionMacros.h>

namespace HOD
{
	namespace RENDERER
	{
		class RenderQueue;
	}

	namespace GAME
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

			virtual void		PushToRenderQueue(RENDERER::RenderQueue& renderQueue);
			virtual void		DrawImGui() = 0;

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
