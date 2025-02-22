#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderCommandSetCameraSettings::RenderCommandSetCameraSettings(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Rect& viewport)
			: RenderCommand()
			, _projectionMatrix(projectionMatrix)
			, _viewMatrix(Matrix4::Inverse(viewMatrix))
			, _viewport(viewport)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderCommandSetCameraSettings::Execute(CommandBuffer* commandBuffer, MaterialInstance* overrideMaterial)
		{
			commandBuffer->SetProjectionMatrix(_projectionMatrix);
			commandBuffer->SetViewMatrix(_viewMatrix);
			commandBuffer->SetViewport(_viewport);
		}
	}
}
