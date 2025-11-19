#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

#include "d3d12.h"

namespace hod::renderer
{
	class HOD_RENDERER_API D3d12Material : public Material
	{
	public:

		D3d12Material();
		~D3d12Material() override;

		bool Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill, Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override; 

	private:

		ComPtr<ID3D12PipelineState> _pipelineState;
	};
}
