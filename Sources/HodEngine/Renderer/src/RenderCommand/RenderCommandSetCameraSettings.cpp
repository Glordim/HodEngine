
#include "RenderCommandSetCameraSettings.h"

#include "RHI/Buffer.h"
#include "RHI/CommandBuffer.h"

#include "Renderer.h"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderCommandSetCameraSettings::RenderCommandSetCameraSettings(const glm::mat4x4& projectionMatrix, const glm::mat4x4& viewMatrix, const CORE::Rect& viewport)
			: RenderCommand()
			, _projectionMatrix(projectionMatrix)
			, _viewMatrix(viewMatrix)
			, _viewport(viewport)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderCommandSetCameraSettings::Execute(CommandBuffer* commandBuffer)
		{
			commandBuffer->SetProjectionMatrix(_projectionMatrix);
			commandBuffer->SetViewMatrix(_viewMatrix);
			commandBuffer->SetViewport(_viewport);
		}
	}
}
