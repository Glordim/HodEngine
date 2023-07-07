#pragma once

#include <vector>

#include "HodEngine/Game/Component.h"

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
			REFLECTED_CLASS(RendererComponent)

		public:

							RendererComponent() = default;
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
