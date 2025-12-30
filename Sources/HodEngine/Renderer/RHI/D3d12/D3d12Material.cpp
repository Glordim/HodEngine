#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Material.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Shader.hpp"
#include "HodEngine/Renderer/RHI/D3d12/RendererDirectX12.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"

namespace hod::renderer
{
	/// @brief
	/// @param type
	D3d12Material::D3d12Material()
	: Material()
	{
	}

	/// @brief
	D3d12Material::~D3d12Material() {}

	DXGI_FORMAT FormatToD3d12Format[VertexInput::Format::Count] = {
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
	};

	bool D3d12Material::Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode /*polygonMode*/,
	                          Topololy topololy, bool /*useDepth*/)
	{
		ComPtr<ID3D12Device5> device = RendererDirectX12::GetInstance()->GetDevice();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.VS = static_cast<D3d12Shader*>(vertexShader)->GetBytecode();
		psoDesc.PS = static_cast<D3d12Shader*>(fragmentShader)->GetBytecode();

		Vector<D3D12_INPUT_ELEMENT_DESC> inputs;
		inputs.Reserve(vertexInputCount);
		for (uint32_t i = 0; i < vertexInputCount; ++i)
		{
			const VertexInput* vertexInput = vertexInputs + i;

			D3D12_INPUT_ELEMENT_DESC inputDesc;
			if (i == 0)
			{
				inputDesc.SemanticName = "POSITION";
			}
			else if (i == 1)
			{
				inputDesc.SemanticName = "TEXCOORD";
			}
			else
			{
				inputDesc.SemanticName = "COLOR";
			}
			inputDesc.SemanticIndex = 0;
			inputDesc.InputSlot = 0;
			inputDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			inputDesc.Format = FormatToD3d12Format[vertexInput->_format];
			inputDesc.AlignedByteOffset = (i == 0) ? 0 : D3D12_APPEND_ALIGNED_ELEMENT;

			inputs.push_back(inputDesc);
		}
		psoDesc.InputLayout.NumElements = (UINT)inputs.Size();
		psoDesc.InputLayout.pInputElementDescs = inputs.Data();

		// RootSignature
		D3D12_DESCRIPTOR_RANGE1 cbvRange = {};
		cbvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRange.NumDescriptors = 1;
		cbvRange.BaseShaderRegister = 0; // register b0
		cbvRange.RegisterSpace = 0;      // espace 0
		cbvRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
		cbvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_DESCRIPTOR_RANGE1 samplerRange = {};
		samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		samplerRange.NumDescriptors = 1;
		samplerRange.BaseShaderRegister = 0; // s0
		samplerRange.RegisterSpace = 0;
		samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		samplerRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

		D3D12_DESCRIPTOR_RANGE1 srvRange = {};
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = 1;
		srvRange.BaseShaderRegister = 0; // t0
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		srvRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

		D3D12_ROOT_PARAMETER1 cbvParam = {};
		cbvParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		cbvParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		cbvParam.DescriptorTable.NumDescriptorRanges = 1;
		cbvParam.DescriptorTable.pDescriptorRanges = &cbvRange;

		D3D12_ROOT_PARAMETER1 samplerParam = {};
		samplerParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		samplerParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		samplerParam.DescriptorTable.NumDescriptorRanges = 1;
		samplerParam.DescriptorTable.pDescriptorRanges = &samplerRange;

		D3D12_ROOT_PARAMETER1 srvParam = {};
		srvParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		srvParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		srvParam.DescriptorTable.NumDescriptorRanges = 1;
		srvParam.DescriptorTable.pDescriptorRanges = &srvRange;

		D3D12_ROOT_PARAMETER1 rootParams[3] = {cbvParam, srvParam, samplerParam};

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSignatureDesc.Desc_1_1.NumParameters = 3;
		rootSignatureDesc.Desc_1_1.pParameters = rootParams;
		rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
		rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
		rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;

		if (FAILED(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signatureBlob, &errorBlob)))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			// Handle error...
		}

		ComPtr<ID3D12RootSignature> myRootSignature;
		device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&myRootSignature));
		psoDesc.pRootSignature = myRootSignature.Get();
		//

		// RasterizerState
		psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // todo back
		psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.DepthClipEnable = true;
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
		psoDesc.RasterizerState.MultisampleEnable = FALSE;
		psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		//

		// BlendState
		D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
		rtBlendDesc.BlendEnable = TRUE;
		rtBlendDesc.LogicOpEnable = FALSE;
		rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesc.BlendState.IndependentBlendEnable = FALSE;
		psoDesc.BlendState.RenderTarget[0] = rtBlendDesc;
		//

		// DepthStencilState
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;   // 0xFF
		psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK; // 0xFF

		psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

		psoDesc.DepthStencilState.BackFace = psoDesc.DepthStencilState.FrontFace;
		//

		psoDesc.SampleMask = UINT_MAX;

		switch (topololy)
		{
			case Topololy::POINT: psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT; break;
			case Topololy::LINE: psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
			case Topololy::LINE_STRIP: psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
			case Topololy::TRIANGLE: psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
			case Topololy::TRIANGLE_FAN: psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;

			default:
				psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED; // todo
				break;
		}
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState))))
		{
			RendererDirectX12::GetInstance()->OutputErrors();
			return false;
		}

		return true;
	}
}
