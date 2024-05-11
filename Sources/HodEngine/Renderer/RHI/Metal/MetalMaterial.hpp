#pragma once

#include "HodEngine/Renderer/RHI/Material.hpp"

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
			virtual										~MetalMaterial();

			virtual bool								Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;
		};
	}
}
