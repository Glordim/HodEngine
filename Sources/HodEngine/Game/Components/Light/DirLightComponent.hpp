#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Color.hpp>
#include <HodEngine/Renderer/Light/DirLight.hpp>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API DirLightComponent : public Component
		{
		public:

								DirLightComponent() = default;
								DirLightComponent(const DirLightComponent&) = delete;
								DirLightComponent(DirLightComponent&&) = delete;
								~DirLightComponent() override = default;

			void				operator=(const DirLightComponent&) = delete;
			void				operator=(DirLightComponent&&) = delete;

		public:

			renderer::DirLight*	GetDirLight() const;

		public:
			renderer::DirLight	_data;
		};
	}
}
