#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"
#include "HodEngine/Renderer/RHI/VertexInput.hpp"

#include <Metal/Metal.hpp>

#include <iostream> // TODO output

namespace hod
{
	namespace renderer
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
		MetalMaterial::MetalMaterial()
		: Material()
		, _vertexAttributeBufferRange(0, 0)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		MetalMaterial::~MetalMaterial()
		{
			_renderPipelineState->release();
		}

		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		bool MetalMaterial::Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode,
		                          Material::Topololy topololy, bool useDepth)
		{
			// TODDO
			(void)topololy;
			(void)polygonMode;
			(void)useDepth;

			MTL::Function* vertexFunction = static_cast<MetalShader*>(vertexShader)->GetNativeFunction();
			MTL::Function* fragmentFunction = static_cast<MetalShader*>(fragmentShader)->GetNativeFunction();
			// MTL::VertexDescriptor* vertexDescriptor = static_cast<MetalVertexInput*>(vertexInputs)->GetNativeVertexDescriptor();

			uint32_t lastUsedBufferIndex = 7; // TODO
			_vertexAttributeBufferRange.location = lastUsedBufferIndex + 1;

			MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

			uint32_t offset = 0;

			if (vertexInputs != nullptr)
			{
				Vector<uint32_t> strides;
				strides.Resize(vertexInputCount, 0);
				for (uint32_t i = 0; i < vertexInputCount; ++i)
				{
					uint32_t bufferIndex = vertexInputs[i]._binding;
					uint32_t offset = vertexInputs[i]._offset;
					uint32_t Size = FormatToSize[vertexInputs[i]._format];

					MTL::VertexAttributeDescriptor* vertexAttribute = MTL::VertexAttributeDescriptor::alloc()->init();
					vertexAttribute->setOffset(offset);
					vertexAttribute->setBufferIndex(_vertexAttributeBufferRange.location + bufferIndex);
					vertexAttribute->setFormat(FormatToMetalFormat[vertexInputs[i]._format]);
					vertexDescriptor->attributes()->setObject(vertexAttribute, i);
					vertexAttribute->release();

					strides[bufferIndex] = std::max(strides[bufferIndex], offset + Size);
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
					// std::cout << "Attribute at index " << attr->attributeIndex() << ": " << attr->name()->utf8String() << std::endl;
					// std::cout << "  Format: " << attr->format() << std::endl;

					std::cout << attribute->name()->utf8String() << std::endl;
					std::cout << attribute->attributeType() << std::endl;

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
			pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
			pipelineDescriptor->colorAttachments()->object(0)->setBlendingEnabled(true);
			pipelineDescriptor->colorAttachments()->object(0)->setRgbBlendOperation(MTL::BlendOperationAdd);
			pipelineDescriptor->colorAttachments()->object(0)->setAlphaBlendOperation(MTL::BlendOperationAdd);
			pipelineDescriptor->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
			pipelineDescriptor->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
			pipelineDescriptor->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactorZero);

			// Create the render pipeline state
			NS::Error*     pipelineError = nil;
			RendererMetal* rendererMetal = RendererMetal::GetInstance();
			_renderPipelineState = rendererMetal->GetDevice()->newRenderPipelineState(pipelineDescriptor, &pipelineError);
			pipelineDescriptor->release();
			vertexDescriptor->release();
			if (_renderPipelineState == nullptr)
			{
				std::cerr << "Failed to create render pipeline state: " << pipelineError->localizedDescription()->utf8String() << std::endl; // TODO Output
				return false;
			}

			return true;
		}

		MTL::RenderPipelineState* MetalMaterial::GetNativeRenderPipeline() const
		{
			return _renderPipelineState;
		}

		NS::Range MetalMaterial::GetVertexAttributeBufferRange() const
		{
			return _vertexAttributeBufferRange;
		}
	}
}
