#pragma once

#include "HodEngine/Renderer/RenderCommand/RenderCommand.h"
#include "HodEngine/Renderer/P2fT2f.h"

#include <vector>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include <HodEngine/Core/Rect.h>

namespace hod
{
	namespace renderer
	{
		class MaterialInstance;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderCommandSetCameraSettings : public RenderCommand
		{
		public:

							RenderCommandSetCameraSettings(const glm::mat4x4& projectionMatrix, const glm::mat4x4& viewMatrix, const CORE::Rect& viewport);
							RenderCommandSetCameraSettings(const RenderCommandSetCameraSettings&) = delete;
							RenderCommandSetCameraSettings(RenderCommandSetCameraSettings&&) = delete;
							~RenderCommandSetCameraSettings() override = default;

			void			operator=(const RenderCommandSetCameraSettings&) = delete;
			void			operator=(RenderCommandSetCameraSettings&&) = delete;

		public:

			void			Execute(CommandBuffer* commandBuffer) override;

		private:

			glm::mat4x4		_projectionMatrix;
			glm::mat4x4		_viewMatrix;
			CORE::Rect		_viewport;
		};
	}
}
