
#include "HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.h"

#include "HodEngine/Renderer/RHI/Buffer.h"
#include "HodEngine/Renderer/RHI/CommandBuffer.h"

#include "HodEngine/Renderer/Renderer.h"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderCommandSetCameraSettings::RenderCommandSetCameraSettings(const glm::mat4x4& projectionMatrix, const glm::mat4x4& viewMatrix, const Rect& viewport)
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
