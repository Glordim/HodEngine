#pragma once
#include "HodEngine/RHI/Export.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"

#include "d3d12.h"
#include <wrl/client.h>

namespace hod::inline rhi
{
	class HOD_RHI_API D3d12GraphicsPipeline : public GraphicsPipeline
	{
	public:
		D3d12GraphicsPipeline();
		~D3d12GraphicsPipeline() override;

		bool Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode = PolygonMode::Fill,
		           Topololy topololy = Topololy::TRIANGLE, bool useDepth = true) override;

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState;
	};
}
