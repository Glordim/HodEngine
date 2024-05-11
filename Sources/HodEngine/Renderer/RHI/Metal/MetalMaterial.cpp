#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalMaterial::MetalMaterial() : Material()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalMaterial::~MetalMaterial()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalMaterial::Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			return true;
		}
	}
}
