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
	class HOD_RHI_API GraphicsPipelineMetal : public GraphicsPipeline
	{
	public:

								GraphicsPipelineMetal();
								~GraphicsPipelineMetal() override;

		bool			        Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;
		
		NS::Range				GetVertexAttributeBufferRange() const;

		MTL::RenderPipelineState*   GetNativeRenderPipeline() const;
		
	private:
		
		MTL::RenderPipelineState*   _renderPipelineState = nullptr;

		NS::Range _vertexAttributeBufferRange;
	};
}
