#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCommandBuffer.hpp"

#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <Metal/Metal.hpp>

#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include <algorithm>
#include <cstring>

namespace hod
{
	namespace renderer
	{
		/// @brief
		MetalCommandBuffer::MetalCommandBuffer()
		{
			RendererMetal* rendererMetal = RendererMetal::GetInstance();

			_commandBuffer = rendererMetal->GetCommandQueue()->commandBuffer();
		}

		/// @brief
		MetalCommandBuffer::~MetalCommandBuffer()
		{
			if (_renderCommandEncoder)
			{
				_renderCommandEncoder->release();
			}
			_commandBuffer->release();
		}

		/// @brief
		/// @return
		bool MetalCommandBuffer::StartRecord()
		{
			return true;
		}

		/// @brief
		/// @return
		bool MetalCommandBuffer::EndRecord()
		{
			return true;
		}

		/// @brief
		/// @return
		bool MetalCommandBuffer::StartRenderPass(RenderTarget* renderTarget, PresentationSurface* presentationSurface, const Color& color)
		{
			(void)renderTarget; // TODO

			MetalPresentationSurface* metalPresentationSurface = static_cast<MetalPresentationSurface*>(presentationSurface);
			CA::MetalDrawable*        drawable = metalPresentationSurface->GetCurrentDrawable();
			MTL::Texture*             drawableTexture = drawable->texture();

			MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
			renderPassDescriptor->colorAttachments()->object(0)->setTexture(drawableTexture);
			renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
			renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
			renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(color.r, color.g, color.b, color.a));

			_renderCommandEncoder = _commandBuffer->renderCommandEncoder(renderPassDescriptor);
			renderPassDescriptor->release();

			_renderPassWidth = (uint32_t)drawableTexture->width();
			_renderPassHeight = (uint32_t)drawableTexture->height();

			return true;
		}

		/// @brief
		/// @return
		bool MetalCommandBuffer::EndRenderPass()
		{
			_renderCommandEncoder->endEncoding();

			return true;
		}

		/// @brief
		/// @param constant
		/// @param Size
		/// @param shaderType
		void MetalCommandBuffer::SetConstant(void* constant, uint32_t Size, Shader::ShaderType shaderType)
		{
			Buffer* constantBuffer = RendererMetal::GetInstance()->CreateBuffer(renderer::Buffer::Usage::Vertex, Size);
			void*   constantBufferData = constantBuffer->Lock();
			if (constantBufferData != nullptr)
			{
				memcpy(constantBufferData, constant, Size);
				constantBuffer->Unlock();
			}
			DeleteAfterRender(constantBuffer);

			if (shaderType == Shader::ShaderType::Vertex)
			{
				_renderCommandEncoder->setVertexBuffer(static_cast<MetalBuffer*>(constantBuffer)->GetNativeBuffer(), 0, 0);
			}
			else if (shaderType == Shader::ShaderType::Fragment)
			{
				_renderCommandEncoder->setFragmentBuffer(static_cast<MetalBuffer*>(constantBuffer)->GetNativeBuffer(), 0, 0);
			}
		}

		/// @brief
		/// @param projectionMatrix
		void MetalCommandBuffer::SetProjectionMatrix(const Matrix4& projectionMatrix)
		{
			// todo
			(void)projectionMatrix;
		}

		/// @brief
		/// @param viewMatrix
		void MetalCommandBuffer::SetViewMatrix(const Matrix4& viewMatrix)
		{
			// todo
			(void)viewMatrix;
		}

		/// @brief
		/// @param modelMatrix
		void MetalCommandBuffer::SetModelMatrix(const Matrix4& modelMatrix)
		{
			// todo
			(void)modelMatrix;
		}

		/// @brief
		/// @param viewport
		void MetalCommandBuffer::SetViewport(const Rect& viewport)
		{
			MTL::Viewport mtlViewport;
			mtlViewport.originX = viewport._position.GetX();
			mtlViewport.originY = viewport._position.GetY();
			mtlViewport.width = viewport._size.GetX();
			mtlViewport.height = viewport._size.GetY();
			mtlViewport.znear = 0;
			mtlViewport.zfar = 1;
			_renderCommandEncoder->setViewport(mtlViewport);
		}

		/// @brief
		/// @param scissor
		void MetalCommandBuffer::SetScissor(const Rect& scissor)
		{
			MTL::ScissorRect scissorRect;
			scissorRect.x = std::clamp((uint32_t)scissor._position.GetX(), 0u, _renderPassWidth);
			scissorRect.y = std::clamp((uint32_t)scissor._position.GetY(), 0u, _renderPassHeight);
			scissorRect.width = std::clamp((uint32_t)scissor._size.GetX(), 0u, _renderPassWidth - (uint32_t)scissorRect.x);
			scissorRect.height = std::clamp((uint32_t)scissor._size.GetY(), 0u, _renderPassHeight - (uint32_t)scissorRect.y);
			_renderCommandEncoder->setScissorRect(scissorRect);
		}

		void MetalCommandBuffer::SetMaterial(const Material* material)
		{
			_material = static_cast<const MetalMaterial*>(material);
			_renderCommandEncoder->setRenderPipelineState(_material->GetNativeRenderPipeline());
		}

		/// @brief
		/// @param materialInstance
		/// @param setOffset
		/// @param setCount
		void MetalCommandBuffer::SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset, uint32_t setCount)
		{
			// TODO
			(void)setOffset;
			(void)setCount;
			//
			static_cast<const MetalMaterialInstance*>(materialInstance)->FillCommandEncoder(_renderCommandEncoder);
		}

		/// @brief
		/// @param vertexBuffer
		/// @param count
		/// @param offset
		void MetalCommandBuffer::SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset)
		{
			MTL::Buffer** mtlBuffers = (MTL::Buffer**)alloca(sizeof(MTL::Buffer*) * count);
			NS::UInteger* bufferOffsets = (NS::UInteger*)alloca(sizeof(NS::UInteger) * count);
			NS::Range     range = _material->GetVertexAttributeBufferRange();

			for (uint32_t index = 0; index < count; ++index)
			{
				mtlBuffers[index] = static_cast<MetalBuffer*>(vertexBuffer[index])->GetNativeBuffer();
				bufferOffsets[index] = offset;
			}
			_renderCommandEncoder->setVertexBuffers(mtlBuffers, bufferOffsets, range);
		}

		/// @brief
		/// @param indexBuffer
		/// @param offset
		void MetalCommandBuffer::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset)
		{
			_indexBuffer = static_cast<MetalBuffer*>(indexBuffer);
			_indexBufferOffset = offset;
		}

		/// @brief
		/// @param vertexCount
		void MetalCommandBuffer::Draw(uint32_t vertexCount)
		{
			// TODO primitive type from Material ?
			_renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, 0, vertexCount, 1);
		}

		/// @brief
		/// @param indexCount
		/// @param indexOffset
		/// @param vertexOffset
		void MetalCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
		{
			(void)vertexOffset; // TODO
			// TODO primitive type from Material ?
			_renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt16, _indexBuffer->GetNativeBuffer(),
			                                             indexOffset * sizeof(uint16_t) + _indexBufferOffset, 1);
		}

		/// @brief
		/// @param presentationSurface
		void MetalCommandBuffer::Present(PresentationSurface* presentationSurface)
		{
			MetalPresentationSurface* metalPresentationSurface = static_cast<MetalPresentationSurface*>(presentationSurface);
			_commandBuffer->presentDrawable(metalPresentationSurface->GetCurrentDrawable());
		}

		/// @brief
		/// @return
		MTL::CommandBuffer* MetalCommandBuffer::GetNativeCommandBuffer() const
		{
			return _commandBuffer;
		}
	}
}
