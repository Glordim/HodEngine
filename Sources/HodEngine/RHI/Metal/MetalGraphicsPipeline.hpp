#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/GraphicsPipeline.hpp"

#include <Foundation/NSRange.hpp>

namespace MTL
{
    class RenderPipelineState;
}

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RHI_API MetalGraphicsPipeline : public GraphicsPipeline
	{
	public:

								MetalGraphicsPipeline();
								~MetalGraphicsPipeline() override;

		bool			        Build(const CreateInfo& createInfo) override;
		
		NS::Range				GetVertexAttributeBufferRange() const;

		MTL::RenderPipelineState*   GetNativeRenderPipeline() const;

		CullMode				GetCullMode() const { return _cullMode; }
		
	private:
		
		MTL::RenderPipelineState*   _renderPipelineState = nullptr;

		NS::Range _vertexAttributeBufferRange;

		CullMode _cullMode = CullMode::None;
	};
}
