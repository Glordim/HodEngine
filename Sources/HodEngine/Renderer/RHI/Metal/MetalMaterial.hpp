#pragma once

#include "HodEngine/Renderer/RHI/Material.hpp"

namespace MTL
{
    class RenderPipelineState;
}

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MetalMaterial : public Material
		{
		public:

									MetalMaterial();
			virtual					~MetalMaterial();

			bool			        Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;
            
            MTL::RenderPipelineState*   GetNativeRenderPipeline() const;
            
        private:
            
            MTL::RenderPipelineState*    _renderPipelineState = nullptr;
		};
	}
}