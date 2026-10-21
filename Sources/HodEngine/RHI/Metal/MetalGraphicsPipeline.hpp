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
		Topology				GetTopology() const { return _topology; }
		PolygonMode				GetPolygonMode() const { return _polygonMode; }

	private:

		MTL::RenderPipelineState*   _renderPipelineState = nullptr;

		NS::Range _vertexAttributeBufferRange;

		// Not part of the pipeline state on Metal: given with each draw, see MetalCommandBuffer
		CullMode    _cullMode = CullMode::None;
		Topology    _topology = Topology::TRIANGLE;
		PolygonMode _polygonMode = PolygonMode::Fill;
	};
}
