#pragma once

#include "HodEngine/Game/src/Component.h"

#include <HodEngine/Core/Color.h>
#include <HodEngine/Renderer/Light/DirLight.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class DirLightComponent : public Component
		{
		public:

								DirLightComponent(const std::weak_ptr<Entity>& entity);
								DirLightComponent(const DirLightComponent&) = delete;
								DirLightComponent(DirLightComponent&&) = delete;
								~DirLightComponent() override = default;

			void				operator=(const DirLightComponent&) = delete;
			void				operator=(DirLightComponent&&) = delete;

		public:

			const char*			GetType() const override;

			renderer::DirLight*	GetDirLight() const;

		public:
			renderer::DirLight	_data;
		};
	}
}
