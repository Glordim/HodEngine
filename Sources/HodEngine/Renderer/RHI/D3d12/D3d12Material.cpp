#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Material.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Shader.hpp"
#include "HodEngine/Renderer/RHI/D3d12/RendererDirectX12.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"

#include <HodEngine/Core/Assert.hpp>

#include <algorithm>

#undef max

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

	enum class UsedStage : uint8_t
	{
		Vertex = (1 << 0),
		Fragment = (1 << 1),
	};

	struct Param
	{
		uint32_t index;
		uint8_t  usedStages;
	};

	struct SpaceReflection
	{
		uint32_t      spaceIndex;
		Vector<Param> contantBuffers;
		Vector<Param> shaderResources;
		Vector<Param> samplerStates;
	};

	void AppendRanges(const Vector<Param>& params, std::underlying_type_t<UsedStage> usedStages, D3D12_DESCRIPTOR_RANGE1 range, Vector<D3D12_DESCRIPTOR_RANGE1>& ranges)
	{
		uint32_t lastIndex = std::numeric_limits<uint32_t>::max();
		for (const Param& param : params)
		{
			if (param.usedStages != usedStages)
			{
				continue;
			}

			if (lastIndex == std::numeric_limits<uint32_t>::max())
			{
				range.BaseShaderRegister = param.index;
				range.NumDescriptors = 0;
			}
			else if (lastIndex != param.index - 1)
			{
				ranges.PushBack(range);
				range.BaseShaderRegister = param.index;
				range.NumDescriptors = 0;
			}
			++range.NumDescriptors;
			lastIndex = param.index;
		}
		if (range.NumDescriptors > 0)
		{
			ranges.PushBack(range);
		}
	}

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

		Vector<SpaceReflection> spaceParams;
		spaceParams.Reserve(4);

		const Document&       vertexShaderReflection = static_cast<D3d12Shader*>(vertexShader)->GetReflection();
		const Document::Node* vertexShaderEntryPointsNode = vertexShaderReflection.GetRootNode().GetChild("entryPoints");
		Assert(vertexShaderEntryPointsNode);
		const Document::Node* vertexShaderEntryPointNode = vertexShaderEntryPointsNode->GetFirstChild();
		Assert(vertexShaderEntryPointNode);
		const Document::Node* vertexShaderEntryPointsBindingNode = vertexShaderEntryPointNode->GetChild("bindings");
		if (vertexShaderEntryPointsBindingNode != nullptr)
		{
			const Document::Node* childNode = vertexShaderEntryPointsBindingNode->GetFirstChild();
			while (childNode != nullptr)
			{
				const Document::Node* bindingNode = childNode->GetChild("binding");
				Assert(bindingNode);

				const Document::Node* usedNode = bindingNode->GetChild("used");
				Assert(usedNode);

				if (usedNode->GetBool() == false)
				{
					childNode = childNode->GetNextSibling();
					continue;
				}

				uint32_t space = 0;
				auto     it = std::find_if(spaceParams.Begin(), spaceParams.End(), [space](const SpaceReflection& spaceReflection) { return spaceReflection.spaceIndex == space; });
				if (it == spaceParams.End())
				{
					SpaceReflection spaceReflection;
					spaceReflection.spaceIndex = space;
					spaceParams.PushBack(spaceReflection);
					it = spaceParams.End() - 1;
				}

				const Document::Node* indexNode = bindingNode->GetChild("index");
				Assert(indexNode);
				uint32_t index = indexNode->GetUInt32();

				Vector<Param>::Iterator paramIt;
				Vector<Param>*          params = nullptr;

				const Document::Node* kindNode = bindingNode->GetChild("kind");
				Assert(kindNode);
				const String& kind = kindNode->GetString();
				if (kind == "constantBuffer")
				{
					params = &it->contantBuffers;
				}
				else if (kind == "shaderResource")
				{
					params = &it->shaderResources;
				}
				else if (kind == "samplerState")
				{
					params = &it->samplerStates;
				}
				else
				{
					Assert(false);
				}

				paramIt = std::find_if(params->Begin(), params->End(), [index](const Param& param) { return param.index == index; });
				if (paramIt == params->End())
				{
					Param param;
					param.index = index;
					params->PushBack(param);
					paramIt = params->Begin() + params->Size();
				}

				paramIt->usedStages |= std::to_underlying(UsedStage::Vertex);

				childNode = childNode->GetNextSibling();
			}
		}

		const Document&       fragmentShaderReflection = static_cast<D3d12Shader*>(fragmentShader)->GetReflection();
		const Document::Node* fragmentShaderEntryPointsNode = fragmentShaderReflection.GetRootNode().GetChild("entryPoints");
		Assert(fragmentShaderEntryPointsNode);
		const Document::Node* fragmentShaderEntryPointNode = fragmentShaderEntryPointsNode->GetFirstChild();
		Assert(fragmentShaderEntryPointNode);
		const Document::Node* fragmentShaderEntryPointsBindingNode = fragmentShaderEntryPointNode->GetChild("bindings");
		if (fragmentShaderEntryPointsBindingNode != nullptr)
		{
			const Document::Node* childNode = fragmentShaderEntryPointsBindingNode->GetFirstChild();
			while (childNode != nullptr)
			{
				const Document::Node* bindingNode = childNode->GetChild("binding");
				Assert(bindingNode);

				const Document::Node* usedNode = bindingNode->GetChild("used");
				Assert(usedNode);

				if (usedNode->GetBool() == false)
				{
					childNode = childNode->GetNextSibling();
					continue;
				}

				uint32_t space = 0;
				auto     it = std::find_if(spaceParams.Begin(), spaceParams.End(), [space](const SpaceReflection& spaceReflection) { return spaceReflection.spaceIndex == space; });
				if (it == spaceParams.End())
				{
					SpaceReflection spaceReflection;
					spaceReflection.spaceIndex = space;
					spaceParams.PushBack(spaceReflection);
					it = spaceParams.End() - 1;
				}

				const Document::Node* indexNode = bindingNode->GetChild("index");
				Assert(indexNode);
				uint32_t index = indexNode->GetUInt32();

				Vector<Param>::Iterator paramIt;
				Vector<Param>*          params = nullptr;

				const Document::Node* kindNode = bindingNode->GetChild("kind");
				Assert(kindNode);
				const String& kind = kindNode->GetString();
				if (kind == "constantBuffer")
				{
					params = &it->contantBuffers;
				}
				else if (kind == "shaderResource")
				{
					params = &it->shaderResources;
				}
				else if (kind == "samplerState")
				{
					params = &it->samplerStates;
				}
				else
				{
					Assert(false);
				}

				paramIt = std::find_if(params->Begin(), params->End(), [index](const Param& param) { return param.index == index; });
				if (paramIt == params->End())
				{
					Param param;
					param.index = index;
					params->PushBack(param);
					paramIt = params->Begin() + params->Size();
				}

				paramIt->usedStages |= std::to_underlying(UsedStage::Fragment);

				childNode = childNode->GetNextSibling();
			}
		}

		Vector<D3D12_DESCRIPTOR_RANGE1> vertexRanges;
		Vector<D3D12_DESCRIPTOR_RANGE1> fragmentRanges;
		Vector<D3D12_DESCRIPTOR_RANGE1> bothRanges;
		for (SpaceReflection& spaceParam : spaceParams)
		{
			D3D12_DESCRIPTOR_RANGE1 range;
			range.RegisterSpace = spaceParam.spaceIndex;
			range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			std::sort(spaceParam.contantBuffers.Begin(), spaceParam.contantBuffers.End(), [](const Param& a, const Param& b) { return a.index < b.index; });
			AppendRanges(spaceParam.contantBuffers, std::to_underlying(UsedStage::Vertex), range, vertexRanges);
			AppendRanges(spaceParam.contantBuffers, std::to_underlying(UsedStage::Fragment), range, fragmentRanges);
			AppendRanges(spaceParam.contantBuffers, std::to_underlying(UsedStage::Vertex) | std::to_underlying(UsedStage::Fragment), range, bothRanges);

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			std::sort(spaceParam.shaderResources.Begin(), spaceParam.shaderResources.End(), [](const Param& a, const Param& b) { return a.index < b.index; });
			AppendRanges(spaceParam.shaderResources, std::to_underlying(UsedStage::Vertex), range, vertexRanges);
			AppendRanges(spaceParam.shaderResources, std::to_underlying(UsedStage::Fragment), range, fragmentRanges);
			AppendRanges(spaceParam.shaderResources, std::to_underlying(UsedStage::Vertex) | std::to_underlying(UsedStage::Fragment), range, bothRanges);

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			std::sort(spaceParam.samplerStates.Begin(), spaceParam.samplerStates.End(), [](const Param& a, const Param& b) { return a.index < b.index; });
			AppendRanges(spaceParam.samplerStates, std::to_underlying(UsedStage::Vertex), range, vertexRanges);
			AppendRanges(spaceParam.samplerStates, std::to_underlying(UsedStage::Fragment), range, fragmentRanges);
			AppendRanges(spaceParam.samplerStates, std::to_underlying(UsedStage::Vertex) | std::to_underlying(UsedStage::Fragment), range, bothRanges);
		}

		// RootSignature
		D3D12_ROOT_PARAMETER1 vertexParam = {};
		vertexParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		vertexParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		vertexParam.DescriptorTable.NumDescriptorRanges = vertexRanges.Size();
		vertexParam.DescriptorTable.pDescriptorRanges = vertexRanges.Data();

		D3D12_ROOT_PARAMETER1 fragmentParam = {};
		fragmentParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		fragmentParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		fragmentParam.DescriptorTable.NumDescriptorRanges = fragmentRanges.Size();
		fragmentParam.DescriptorTable.pDescriptorRanges = fragmentRanges.Data();

		D3D12_ROOT_PARAMETER1 allParam = {};
		allParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		allParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		allParam.DescriptorTable.NumDescriptorRanges = bothRanges.Size();
		allParam.DescriptorTable.pDescriptorRanges = bothRanges.Data();

		D3D12_ROOT_PARAMETER1 rootParams[3] = {vertexParam, fragmentParam, allParam};

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
