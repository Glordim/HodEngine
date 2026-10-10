#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Math/Matrix4.hpp"

#include <cstdint>

namespace hod::inline rhi
{
	class CommandBuffer;
}

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API RenderCommand
	{
	public:

							RenderCommand() = default;
							RenderCommand(const RenderCommand&) = delete;
							RenderCommand(RenderCommand&&) = delete;
		virtual				~RenderCommand() = default;

		void				operator=(const RenderCommand&) = delete;
		void				operator=(RenderCommand&&) = delete;

	public:

		// projection, view: the camera of the RenderView the command is drawn in
		virtual void		Execute(CommandBuffer* commandBuffer, const Matrix4& projection, const Matrix4& view, MaterialInstance* overrideMaterial = nullptr) = 0;
		virtual uint32_t	GetRenderingOrder() const = 0;
	};
}
