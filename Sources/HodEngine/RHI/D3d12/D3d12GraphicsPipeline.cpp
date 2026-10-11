#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/D3d12/D3d12GraphicsPipeline.hpp"

#include "HodEngine/RHI/D3d12/D3d12RhiDevice.hpp"
#include "HodEngine/RHI/D3d12/D3d12Shader.hpp"

#include "HodEngine/RHI/ShaderConstantDescriptor.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/VertexInput.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <algorithm>

namespace hod::inline rhi
{
	namespace
	{
		DXGI_FORMAT FormatToDxgiFormat[VertexInput::Format::Count] = {
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
		};

		uint32_t FormatToByteSize[VertexInput::Format::Count] = {
			1 * sizeof(float),
			2 * sizeof(float),
			4 * sizeof(float),
			1 * sizeof(uint32_t),
		};
	}

	/// @brief
	D3d12GraphicsPipeline::D3d12GraphicsPipeline()
	: GraphicsPipeline()
	{
	}

	/// @brief
	D3d12GraphicsPipeline::~D3d12GraphicsPipeline()
	{
		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		rhiDevice->DeferRelease(std::move(_pipelineState));
		rhiDevice->DeferRelease(std::move(_rootSignature));
	}

	/// @brief
	/// @param createInfo
	/// @return
	bool D3d12GraphicsPipeline::Build(const CreateInfo& createInfo)
	{
		const VertexInput* vertexInputs = createInfo._vertexInputs;
		uint32_t           vertexInputCount = createInfo._vertexInputCount;
		Shader*            vertexShader = createInfo._vertexShader;
		Shader*            fragmentShader = createInfo._fragmentShader;

		D3d12RhiDevice* rhiDevice = D3d12RhiDevice::GetInstance();

		MergeSetDescriptors(*vertexShader, *fragmentShader);

		if (BuildRootSignature(vertexShader, fragmentShader) == false)
		{
			return false;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
		pipelineStateDesc.pRootSignature = _rootSignature.Get();
		pipelineStateDesc.VS = static_cast<D3d12Shader*>(vertexShader)->GetBytecode();
		pipelineStateDesc.PS = static_cast<D3d12Shader*>(fragmentShader)->GetBytecode();

		// Input layout
		// VertexInput i is the i-th input the vertex shader declares. It follows the previous ones of its binding in the vertex buffer.
		const Vector<D3d12Shader::VertexInputSemantic>& semantics = static_cast<D3d12Shader*>(vertexShader)->GetVertexInputSemantics();

		Vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
		inputElements.Reserve(vertexInputCount);
		_vertexStrides.Clear();
		for (uint32_t i = 0; i < vertexInputCount; ++i)
		{
			const VertexInput& vertexInput = vertexInputs[i];
			if (vertexInput._binding >= D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)
			{
				OUTPUT_ERROR("D3d12: GraphicsPipeline, vertex input binding {} is out of range", vertexInput._binding);
				return false;
			}
			if (vertexInput._binding >= _vertexStrides.Size())
			{
				_vertexStrides.Resize(vertexInput._binding + 1, 0);
			}

			if (i < semantics.Size()) // the others are not read by the shader
			{
				D3D12_INPUT_ELEMENT_DESC inputElement = {};
				inputElement.SemanticName = semantics[i]._name.CStr();
				inputElement.SemanticIndex = semantics[i]._index;
				inputElement.Format = FormatToDxgiFormat[vertexInput._format];
				inputElement.InputSlot = vertexInput._binding;
				inputElement.AlignedByteOffset = _vertexStrides[vertexInput._binding];
				inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				inputElement.InstanceDataStepRate = 0;
				inputElements.PushBack(inputElement);
			}

			_vertexStrides[vertexInput._binding] += FormatToByteSize[vertexInput._format];
		}
		pipelineStateDesc.InputLayout.NumElements = inputElements.Size();
		pipelineStateDesc.InputLayout.pInputElementDescs = inputElements.Data();

		// Input assembly
		_triangleFanEmulated = false;
		switch (createInfo._topololy)
		{
			case Topololy::POINT:
				pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
				_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
				break;

			case Topololy::LINE:
				pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
				break;

			case Topololy::LINE_STRIP:
				pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
				break;

			case Topololy::TRIANGLE_FAN:
				pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				if (rhiDevice->IsTriangleFanSupported())
				{
					_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLEFAN;
				}
				else
				{
					_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					_triangleFanEmulated = true;
				}
				break;

			case Topololy::TRIANGLE:
			default:
				pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				break;
		}

		// Rasterizer
		if (createInfo._polygonMode == PolygonMode::Fill)
		{
			pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		}
		else if (createInfo._polygonMode == PolygonMode::Line)
		{
			pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		}
		else
		{
			OUTPUT_ERROR("D3d12: GraphicsPipeline, PolygonMode::Point is not supported");
			return false;
		}
		if (createInfo._cullMode == CullMode::Back)
		{
			pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		}
		else if (createInfo._cullMode == CullMode::Front)
		{
			pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		}
		else
		{
			pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		}
		pipelineStateDesc.RasterizerState.FrontCounterClockwise = TRUE;
		pipelineStateDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		pipelineStateDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		pipelineStateDesc.RasterizerState.DepthClipEnable = TRUE;
		pipelineStateDesc.RasterizerState.MultisampleEnable = FALSE;
		pipelineStateDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		pipelineStateDesc.RasterizerState.ForcedSampleCount = 0;
		pipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// Multisampling
		pipelineStateDesc.SampleMask = UINT_MAX;
		pipelineStateDesc.SampleDesc.Count = 1;

		// Depth and stencil testing
		// The render passes have no depth attachment, on any backend: what is drawn is ordered by the render queues
		pipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
		pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

		// Color blending
		D3D12_RENDER_TARGET_BLEND_DESC& renderTargetBlend = pipelineStateDesc.BlendState.RenderTarget[0];
		renderTargetBlend.BlendEnable = createInfo._blendMode != BlendMode::Opaque ? TRUE : FALSE;
		renderTargetBlend.LogicOpEnable = FALSE;
		if (createInfo._blendMode == BlendMode::PremultipliedAlpha)
		{
			renderTargetBlend.SrcBlend = D3D12_BLEND_ONE;
			renderTargetBlend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			renderTargetBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
			renderTargetBlend.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		}
		else if (createInfo._blendMode == BlendMode::Additive)
		{
			renderTargetBlend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			renderTargetBlend.DestBlend = D3D12_BLEND_ONE;
			renderTargetBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
			renderTargetBlend.DestBlendAlpha = D3D12_BLEND_ONE;
		}
		else // Alpha, and what Opaque leaves unused
		{
			renderTargetBlend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			renderTargetBlend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			renderTargetBlend.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
			renderTargetBlend.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		}
		renderTargetBlend.BlendOp = D3D12_BLEND_OP_ADD;
		renderTargetBlend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		renderTargetBlend.LogicOp = D3D12_LOGIC_OP_NOOP;
		renderTargetBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		pipelineStateDesc.BlendState.AlphaToCoverageEnable = FALSE;
		pipelineStateDesc.BlendState.IndependentBlendEnable = FALSE;

		// Every render target and presentation surface has this format, one pipeline state serves them all
		pipelineStateDesc.NumRenderTargets = 1;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (FAILED(rhiDevice->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&_pipelineState))))
		{
			OUTPUT_ERROR("D3d12: Unable to create graphics pipeline!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @param vertexShader
	/// @param fragmentShader
	/// @return
	bool D3d12GraphicsPipeline::BuildRootSignature(Shader* vertexShader, Shader* fragmentShader)
	{
		const D3d12Shader& d3d12VertexShader = *static_cast<D3d12Shader*>(vertexShader);
		const D3d12Shader& d3d12FragmentShader = *static_cast<D3d12Shader*>(fragmentShader);

		Vector<D3D12_ROOT_PARAMETER> rootParameters;

		// The root parameters point to their ranges: these must not move while they are added
		Vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
		uint32_t                       descriptorRangeCount = 0;
		for (const auto& pair : _setDescriptors)
		{
			descriptorRangeCount += pair.second->GetTextureBlocks().Size();
		}
		descriptorRanges.Reserve(descriptorRangeCount);

		// Push constant
		_pushConstantRootParameter = NoRootParameter;
		_pushConstantSize = 0;

		const D3d12Shader::Register* pushConstantRegister = nullptr;
		for (const D3d12Shader* shader : {&d3d12VertexShader, &d3d12FragmentShader})
		{
			const ShaderConstantDescriptor* constantDescriptor = shader->GetConstantDescriptor();
			if (constantDescriptor == nullptr || shader->GetPushConstantRegister() == nullptr)
			{
				continue;
			}

			if (pushConstantRegister != nullptr && *pushConstantRegister != *shader->GetPushConstantRegister())
			{
				OUTPUT_ERROR("D3d12: GraphicsPipeline, the push constant is not on the same register in the vertex and the fragment shader");
				return false;
			}
			pushConstantRegister = shader->GetPushConstantRegister();
			_pushConstantSize = std::max(_pushConstantSize, constantDescriptor->GetSize());
		}

		if (pushConstantRegister != nullptr)
		{
			D3D12_ROOT_PARAMETER rootParameter = {};
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootParameter.Constants.ShaderRegister = pushConstantRegister->_index;
			rootParameter.Constants.RegisterSpace = pushConstantRegister->_space;
			rootParameter.Constants.Num32BitValues = (_pushConstantSize + 3) / 4;

			_pushConstantRootParameter = rootParameters.Size();
			rootParameters.PushBack(rootParameter);
		}

		// _setDescriptors is ordered and contiguous: its entries are the sets 0, 1, 2...
		// The register space of a set is its space, and the binding of a block its register in that space.
		_setLayouts.Clear();
		for (const auto& pair : _setDescriptors)
		{
			SetLayout setLayout;

			for (const ShaderSetDescriptor::BlockUbo& ubo : pair.second->GetUboBlocks())
			{
				D3D12_ROOT_PARAMETER rootParameter = {};
				rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
				rootParameter.Descriptor.ShaderRegister = ubo._binding;
				rootParameter.Descriptor.RegisterSpace = pair.second->GetSpace();

				if (setLayout._firstUniformBufferRootParameter == NoRootParameter)
				{
					setLayout._firstUniformBufferRootParameter = rootParameters.Size();
				}
				rootParameters.PushBack(rootParameter);
			}

			// The ranges of a table have to follow each other: first the ones of the textures, then the ones of the samplers
			for (ShaderSetDescriptor::BlockTexture::Type blockType : {ShaderSetDescriptor::BlockTexture::Texture, ShaderSetDescriptor::BlockTexture::Sampler})
			{
				bool      isTexture = blockType == ShaderSetDescriptor::BlockTexture::Texture;
				uint32_t  firstDescriptorRange = descriptorRanges.Size();
				uint32_t& descriptorCount = isTexture ? setLayout._textureCount : setLayout._samplerCount;

				for (const ShaderSetDescriptor::BlockTexture& textureBlock : pair.second->GetTextureBlocks())
				{
					if (textureBlock._type == ShaderSetDescriptor::BlockTexture::Combined)
					{
						OUTPUT_ERROR("D3d12: GraphicsPipeline, \"{}\" is a combined texture and sampler, which is not supported", textureBlock._name);
						return false;
					}
					if (textureBlock._type != blockType)
					{
						continue;
					}

					D3D12_DESCRIPTOR_RANGE descriptorRange = {};
					descriptorRange.RangeType = isTexture ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV : D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
					descriptorRange.NumDescriptors = 1;
					descriptorRange.BaseShaderRegister = textureBlock._binding;
					descriptorRange.RegisterSpace = pair.second->GetSpace();
					descriptorRange.OffsetInDescriptorsFromTableStart = descriptorCount;
					descriptorRanges.PushBack(descriptorRange);
					++descriptorCount;
				}

				if (descriptorCount > 0)
				{
					D3D12_ROOT_PARAMETER rootParameter = {};
					rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
					rootParameter.DescriptorTable.NumDescriptorRanges = descriptorCount;
					rootParameter.DescriptorTable.pDescriptorRanges = descriptorRanges.Data() + firstDescriptorRange;

					(isTexture ? setLayout._textureTableRootParameter : setLayout._samplerTableRootParameter) = rootParameters.Size();
					rootParameters.PushBack(rootParameter);
				}
			}

			_setLayouts.PushBack(setLayout);
		}

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.NumParameters = rootParameters.Size();
		rootSignatureDesc.pParameters = rootParameters.Data();
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> rootSignatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSignatureBlob, &errorBlob)))
		{
			OUTPUT_ERROR("D3d12: Unable to serialize root signature! {}", errorBlob != nullptr ? static_cast<const char*>(errorBlob->GetBufferPointer()) : "");
			return false;
		}

		ID3D12Device* device = D3d12RhiDevice::GetInstance()->GetDevice();
		if (FAILED(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&_rootSignature))))
		{
			OUTPUT_ERROR("D3d12: Unable to create root signature!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	ID3D12PipelineState* D3d12GraphicsPipeline::GetPipelineState() const
	{
		return _pipelineState.Get();
	}

	/// @brief
	/// @return
	ID3D12RootSignature* D3d12GraphicsPipeline::GetRootSignature() const
	{
		return _rootSignature.Get();
	}

	/// @brief
	/// @return
	D3D_PRIMITIVE_TOPOLOGY D3d12GraphicsPipeline::GetPrimitiveTopology() const
	{
		return _primitiveTopology;
	}

	/// @brief
	/// @return
	bool D3d12GraphicsPipeline::IsTriangleFanEmulated() const
	{
		return _triangleFanEmulated;
	}

	/// @brief
	/// @param set
	/// @return
	const D3d12GraphicsPipeline::SetLayout* D3d12GraphicsPipeline::GetSetLayout(uint32_t set) const
	{
		if (set >= _setLayouts.Size())
		{
			return nullptr;
		}
		return &_setLayouts[set];
	}

	/// @brief
	/// @return
	uint32_t D3d12GraphicsPipeline::GetPushConstantRootParameter() const
	{
		return _pushConstantRootParameter;
	}

	/// @brief
	/// @return
	uint32_t D3d12GraphicsPipeline::GetPushConstantSize() const
	{
		return _pushConstantSize;
	}

	/// @brief
	/// @param binding
	/// @return
	uint32_t D3d12GraphicsPipeline::GetVertexStride(uint32_t binding) const
	{
		if (binding >= _vertexStrides.Size())
		{
			return 0;
		}
		return _vertexStrides[binding];
	}
}
