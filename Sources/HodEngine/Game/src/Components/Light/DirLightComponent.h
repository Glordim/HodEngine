#pragma once


#include "../../Component.h"

#include <HodEngine/Core/Src/Color.h>
#include <HodEngine/Renderer/src/Light/DirLight.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class DirLightComponent : public Component
		{
		public:

								DirLightComponent(Actor* actor);
								DirLightComponent(const DirLightComponent&) = delete;
								DirLightComponent(DirLightComponent&&) = delete;
								~DirLightComponent() override = default;

			void				operator=(const DirLightComponent&) = delete;
			void				operator=(DirLightComponent&&) = delete;

		public:

			void				DrawImGui() override;
			const char*			GetType() const override;

			RENDERER::DirLight*	GetDirLight() const;

		public:
			RENDERER::DirLight	_data;
		};
	}
}