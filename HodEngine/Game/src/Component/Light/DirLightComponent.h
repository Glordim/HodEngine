#pragma once

#include <Core/src/Color.h>

#include "../../Component.h"

#include <Renderer/src/Light/DirLight.h>

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

			DirLight*			GetDirLight() const;

		public:
			DirLight			_data;
		};
	}
}
