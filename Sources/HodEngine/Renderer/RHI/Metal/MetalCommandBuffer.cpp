#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"

#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"

#include <Metal/Metal.hpp>

#include "HodEngine/Renderer/RHI/Metal/MetalCpp/QuartzCore/CAMetalLayer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCpp/QuartzCore/CAMetalDrawable.hpp"

#include <cstring>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalCommandBuffer::MetalCommandBuffer()
		{
			RendererMetal* rendererMetal = RendererMetal::GetInstance();

			_commandBuffer = rendererMetal->GetCommandQueue()->commandBuffer();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalCommandBuffer::~MetalCommandBuffer()
		{
			_commandBuffer->release();
		}

		/// @brief 
		/// @return 
		bool MetalCommandBuffer::StartRecord(RenderTarget* renderTarget, Context* context, const Color& color)
		{
			MetalContext* metalContext = static_cast<MetalContext*>(context);
            CA::MetalDrawable* drawable = metalContext->GetCurrentDrawable();
            MTL::Texture* drawableTexture = drawable->texture();

			MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
			renderPassDescriptor->colorAttachments()->object(0)->setTexture(drawableTexture);
			renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
			renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
			renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(color.r, color.g, color.b, color.a));
			
			_renderCommandEncoder = _commandBuffer->renderCommandEncoder(renderPassDescriptor);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalCommandBuffer::EndRecord()
		{
			_renderCommandEncoder->endEncoding();

			return true;
		}

		/// @brief 
		/// @param constant 
		/// @param size 
		/// @param shaderType 
		void MetalCommandBuffer::SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType)
		{
            Buffer* constantBuffer = RendererMetal::GetInstance()->CreateBuffer(renderer::Buffer::Usage::Vertex, size);
            void* constantBufferData = constantBuffer->Lock();
            if (constantBufferData != nullptr)
            {
                memcpy(constantBufferData, constant, size);
                constantBuffer->Unlock();
            }
            DeleteAfterRender(constantBuffer);
            
            if (shaderType == Shader::ShaderType::Vertex)
            {
                _renderCommandEncoder->setVertexBuffer(static_cast<MetalBuffer*>(constantBuffer)->GetNativeBuffer(), 0, 1);
            }
            else if (shaderType == Shader::ShaderType::Fragment)
            {
                _renderCommandEncoder->setFragmentBuffer(static_cast<MetalBuffer*>(constantBuffer)->GetNativeBuffer(), 0, 1);
            }
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetProjectionMatrix(const Matrix4& projectionMatrix)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetViewMatrix(const Matrix4& viewMatrix)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetModelMatrix(const Matrix4& modelMatrix)
		{
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
            scissorRect.x = scissor._position.GetX();
            scissorRect.y = scissor._position.GetY();
            scissorRect.width = scissor._size.GetX();
            scissorRect.height = scissor._size.GetY();
            _renderCommandEncoder->setScissorRect(scissorRect);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset, uint32_t setCount)
		{
            static_cast<const MetalMaterialInstance*>(materialInstance)->FillCommandEncoder(_renderCommandEncoder);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset)
		{
			// todo count
            _renderCommandEncoder->setVertexBuffer(static_cast<MetalBuffer*>(vertexBuffer[0])->GetNativeBuffer(), offset, 0);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset)
		{
            _indexBuffer = static_cast<MetalBuffer*>(indexBuffer);
            _indexBufferOffset = offset;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::Draw(uint32_t vertexCount)
		{
            // TODO primitive type from Material ?
            _renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, 0, vertexCount, 1);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
		{
            // TODO primitive type from Material ?
            _renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt16, _indexBuffer->GetNativeBuffer(), _indexBufferOffset, 1);
		}

		void MetalCommandBuffer::Present(Context* context)
		{
			MetalContext* metalContext = static_cast<MetalContext*>(context);
			_commandBuffer->presentDrawable(metalContext->GetCurrentDrawable());
		}

		MTL::CommandBuffer*	MetalCommandBuffer::GetNativeCommandBuffer() const
		{
			return _commandBuffer;
		}
	}
}
