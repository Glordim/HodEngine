#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/Core/Math.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/RHI/Metal/MetalGraphicsPipeline.hpp"
#include "HodEngine/RHI/Metal/MetalShader.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"
#include "HodEngine/RHI/VertexInput.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	MTL::VertexFormat FormatToMetalFormat[VertexInput::Format::Count] = {
		MTL::VertexFormatFloat,
		MTL::VertexFormatFloat2,
		MTL::VertexFormatFloat4,
		MTL::VertexFormatUChar4Normalized,
		// VK_FORMAT_A8B8G8R8_UNORM_PACK32,
	};

	uint32_t FormatToSize[VertexInput::Format::Count] = {
		1 * sizeof(float),
		2 * sizeof(float),
		4 * sizeof(float),
		1 * sizeof(uint32_t),
	};

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MetalGraphicsPipeline::MetalGraphicsPipeline()
	: GraphicsPipeline()
	, _vertexAttributeBufferRange(0, 0)
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MetalGraphicsPipeline::~MetalGraphicsPipeline()
	{
		MetalRhiDevice::GetInstance()->DeferRelease(_renderPipelineState);
		MetalRhiDevice::GetInstance()->DeferRelease(_depthStencilState);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool MetalGraphicsPipeline::Build(const CreateInfo& createInfo)
	{
		if (SetTargetFormats(createInfo) == false)
		{
			return false;
		}

		// Set with the pipeline or with each draw, see MetalCommandBuffer
		_cullMode = createInfo._cullMode;
		_topology = createInfo._topology;
		_polygonMode = createInfo._polygonMode;

		const VertexInput* vertexInputs = createInfo._vertexInputs;
		uint32_t           vertexInputCount = createInfo._vertexInputCount;
		Shader*            vertexShader = createInfo._vertexShader;
		Shader*            fragmentShader = createInfo._fragmentShader;

		MTL::Function* vertexFunction = static_cast<MetalShader*>(vertexShader)->GetNativeFunction();
		MTL::Function* fragmentFunction = static_cast<MetalShader*>(fragmentShader)->GetNativeFunction();
		// MTL::VertexDescriptor* vertexDescriptor = static_cast<MetalVertexInput*>(vertexInputs)->GetNativeVertexDescriptor();

		uint32_t lastUsedBufferIndex = 7; // TODO
		_vertexAttributeBufferRange.location = lastUsedBufferIndex + 1;

		MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

		uint32_t offset = 0;

		if (vertexInputs != nullptr)
		{
			uint32_t maxBinding = 0;
			for (uint32_t i = 0; i < vertexInputCount; ++i)
			{
				maxBinding = Max(maxBinding, vertexInputs[i]._binding);
			}

			Vector<uint32_t> strides;
			strides.Resize(maxBinding + 1, 0);
			for (uint32_t i = 0; i < vertexInputCount; ++i)
			{
				uint32_t bufferIndex = vertexInputs[i]._binding;

				// As on the other backends, the attributes of a buffer follow each other in the order they are given
				MTL::VertexAttributeDescriptor* vertexAttribute = MTL::VertexAttributeDescriptor::alloc()->init();
				vertexAttribute->setOffset(strides[bufferIndex]);
				vertexAttribute->setBufferIndex(_vertexAttributeBufferRange.location + bufferIndex);
				vertexAttribute->setFormat(FormatToMetalFormat[vertexInputs[i]._format]);
				vertexDescriptor->attributes()->setObject(vertexAttribute, i);
				vertexAttribute->release();

				strides[bufferIndex] += FormatToSize[vertexInputs[i]._format];
			}

			_vertexAttributeBufferRange.length = 0;
			for (uint32_t i = 0; i < strides.Size(); ++i)
			{
				if (strides[i] == 0)
				{
					continue;
				}

				MTL::VertexBufferLayoutDescriptor* layout = MTL::VertexBufferLayoutDescriptor::alloc()->init();
				layout->setStride(strides[i]);
				vertexDescriptor->layouts()->setObject(layout, _vertexAttributeBufferRange.location + i);
				layout->release();

				++_vertexAttributeBufferRange.length;
			}
		}
		else
		{
			assert(false);
			NS::Array* stageInputs = vertexFunction->stageInputAttributes();
			for (NS::UInteger i = 0; i < stageInputs->count(); ++i)
			{
				MTL::Attribute* attribute = static_cast<MTL::Attribute*>(stageInputs->object(i));
				// OUTPUT_MESSAGE("Attribute at index {}: {}", attr->attributeIndex(), attr->name()->utf8String());
				// OUTPUT_MESSAGE("  Format: {}", attr->format());

				OUTPUT_MESSAGE("{}", attribute->name()->utf8String());
				OUTPUT_MESSAGE("{}", static_cast<int>(attribute->attributeType()));

				MTL::VertexAttributeDescriptor* vertexAttribute = MTL::VertexAttributeDescriptor::alloc()->init();
				vertexAttribute->setOffset(offset);
				vertexAttribute->setBufferIndex(0);

				if (attribute->attributeType() == MTL::DataTypeFloat2)
				{
					vertexAttribute->setFormat(MTL::VertexFormatFloat2);
					offset += sizeof(float) * 2;
				}
				else if (attribute->attributeType() == MTL::DataTypeFloat4)
				{
					vertexAttribute->setFormat(MTL::VertexFormatFloat4);
					offset += sizeof(float) * 4;
				}

				vertexDescriptor->attributes()->setObject(vertexAttribute, i);
				vertexAttribute->release();
			}
		}

		// Create a pipeline descriptor
		MTL::RenderPipelineDescriptor* pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
		pipelineDescriptor->setVertexFunction(vertexFunction);
		pipelineDescriptor->setFragmentFunction(fragmentFunction);
		pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
		// The formats of the render passes it is drawn in, invalid for no attachment of that kind
		pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat((MTL::PixelFormat)MetalTexture::ToMetalPixelFormat(createInfo._colorFormat));
		pipelineDescriptor->setDepthAttachmentPixelFormat((MTL::PixelFormat)MetalTexture::ToMetalPixelFormat(createInfo._depthFormat));
		pipelineDescriptor->colorAttachments()->object(0)->setBlendingEnabled(createInfo._colorFormat != PixelFormat::None && createInfo._blendMode != BlendMode::Opaque);
		pipelineDescriptor->colorAttachments()->object(0)->setRgbBlendOperation(MTL::BlendOperationAdd);
		pipelineDescriptor->colorAttachments()->object(0)->setAlphaBlendOperation(MTL::BlendOperationAdd);
		if (createInfo._blendMode == BlendMode::PremultipliedAlpha)
		{
			pipelineDescriptor->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorOne);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
			pipelineDescriptor->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
		}
		else if (createInfo._blendMode == BlendMode::Additive)
		{
			pipelineDescriptor->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOne);
			pipelineDescriptor->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactorOne);
		}
		else // Alpha, and what Opaque leaves unused
		{
			pipelineDescriptor->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
			pipelineDescriptor->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactorZero);
		}

		// Create the render pipeline state
		NS::Error*     pipelineError = nil;
		MetalRhiDevice* metalRhiDevice = MetalRhiDevice::GetInstance();
		_renderPipelineState = metalRhiDevice->GetDevice()->newRenderPipelineState(pipelineDescriptor, &pipelineError);
		pipelineDescriptor->release();
		vertexDescriptor->release();
		if (_renderPipelineState == nullptr)
		{
			OUTPUT_ERROR("Failed to create render pipeline state: {}", pipelineError->localizedDescription()->utf8String());
			return false;
		}

		// As on the other backends: the depth is only written by the depth test, writing it without testing takes a test that always passes
		MTL::DepthStencilDescriptor* depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
		depthStencilDescriptor->setDepthCompareFunction(createInfo._depthTest ? MTL::CompareFunctionLess : MTL::CompareFunctionAlways);
		depthStencilDescriptor->setDepthWriteEnabled(createInfo._depthWrite);
		_depthStencilState = metalRhiDevice->GetDevice()->newDepthStencilState(depthStencilDescriptor);
		depthStencilDescriptor->release();
		if (_depthStencilState == nullptr)
		{
			OUTPUT_ERROR("Metal: Failed to create depth stencil state");
			return false;
		}

		MergeSetDescriptors(*vertexShader, *fragmentShader);

		return true;
	}

	MTL::RenderPipelineState* MetalGraphicsPipeline::GetNativeRenderPipeline() const
	{
		return _renderPipelineState;
	}

	MTL::DepthStencilState* MetalGraphicsPipeline::GetNativeDepthStencilState() const
	{
		return _depthStencilState;
	}

	NS::Range MetalGraphicsPipeline::GetVertexAttributeBufferRange() const
	{
		return _vertexAttributeBufferRange;
	}
}
