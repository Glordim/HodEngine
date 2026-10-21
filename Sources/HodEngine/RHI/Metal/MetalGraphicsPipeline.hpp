#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/GraphicsPipeline.hpp"

#include <Foundation/NSRange.hpp>

namespace MTL
{
    class DepthStencilState;
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
		// Not part of the pipeline state on Metal: set with it, see MetalCommandBuffer::SetGraphicsPipeline
		MTL::DepthStencilState*     GetNativeDepthStencilState() const;

		// The buffer index each stage reads the constant from, NoConstant when its shader has none. They can differ: the stages are compiled apart.
		static constexpr uint32_t NoConstant = UINT32_MAX;
		uint32_t				GetVertexConstantIndex() const { return _vertexConstantIndex; }
		uint32_t				GetFragmentConstantIndex() const { return _fragmentConstantIndex; }

		CullMode				GetCullMode() const { return _cullMode; }
		Topology				GetTopology() const { return _topology; }
		PolygonMode				GetPolygonMode() const { return _polygonMode; }

	private:

		MTL::RenderPipelineState*   _renderPipelineState = nullptr;
		MTL::DepthStencilState*     _depthStencilState = nullptr;

		NS::Range _vertexAttributeBufferRange;

		uint32_t _vertexConstantIndex = NoConstant;
		uint32_t _fragmentConstantIndex = NoConstant;

		// Not part of the pipeline state on Metal: given with each draw, see MetalCommandBuffer
		CullMode    _cullMode = CullMode::None;
		Topology    _topology = Topology::TRIANGLE;
		PolygonMode _polygonMode = PolygonMode::Fill;
	};
}
