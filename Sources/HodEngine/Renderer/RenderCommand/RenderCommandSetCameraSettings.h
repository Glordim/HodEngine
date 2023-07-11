#pragma once

#include "HodEngine/Renderer/RenderCommand/RenderCommand.h"
#include "HodEngine/Renderer/P2fT2f.h"

#include <vector>

#include <HodEngine/Core/Rect.h>
#include "HodEngine/Core/Math/Matrix4.h"

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

							RenderCommandSetCameraSettings(const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Rect& viewport);
							RenderCommandSetCameraSettings(const RenderCommandSetCameraSettings&) = delete;
							RenderCommandSetCameraSettings(RenderCommandSetCameraSettings&&) = delete;
							~RenderCommandSetCameraSettings() override = default;

			void			operator=(const RenderCommandSetCameraSettings&) = delete;
			void			operator=(RenderCommandSetCameraSettings&&) = delete;

		public:

			void			Execute(CommandBuffer* commandBuffer) override;

		private:

			Matrix4			_projectionMatrix;
			Matrix4			_viewMatrix;
			Rect			_viewport;
		};
	}
}
