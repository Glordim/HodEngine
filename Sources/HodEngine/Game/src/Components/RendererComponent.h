#pragma once

#include <vector>

#include "../Component.h"

namespace hod
{
	namespace game
	{
		class CameraComponent;
		class PointLightComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RendererComponent : public Component
		{
			REFLECTED_ABSTRACT_DERIVED_CLASS(RendererComponent, Component)
			META_TYPE(RendererComponent)

		public:

							RendererComponent(Actor* actor);
							RendererComponent(const RendererComponent&) = delete;
							RendererComponent(RendererComponent&&) = delete;
							~RendererComponent() override = default;

			void			operator=(const RendererComponent&) = delete;
			void			operator=(RendererComponent&&) = delete;

		public:

			const char*		GetType() const override;

			virtual void	Render() = 0;
		};
	}
}
