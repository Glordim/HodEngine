#pragma once
#include "HodEngine/Renderer/Export.hpp"

namespace hod
{
	namespace renderer
	{
		class CommandBuffer;
		class MaterialInstance;

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

			virtual void		Execute(CommandBuffer* commandBuffer, MaterialInstance* overrideMaterial = nullptr) = 0;
			virtual uint32_t	GetRenderingOrder() const = 0;
		};
	}
}
