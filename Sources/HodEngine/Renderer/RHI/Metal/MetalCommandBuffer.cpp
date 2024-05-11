
#include "HodEngine/Renderer/RHI/Metal/MetalCommandBuffer.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalCommandBuffer::MetalCommandBuffer()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MetalCommandBuffer::~MetalCommandBuffer()
		{
		}

		/// @brief 
		/// @return 
		bool MetalCommandBuffer::StartRecord(RenderTarget* renderTarget, Context* context)
		{
			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool MetalCommandBuffer::EndRecord()
		{
			return true;
		}

		/// @brief 
		/// @param constant 
		/// @param size 
		/// @param shaderType 
		void MetalCommandBuffer::SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType)
		{
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
		}

		/// @brief 
		/// @param scissor 
		void MetalCommandBuffer::SetScissor(const Rect& scissor)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset, uint32_t setCount)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetVertexBuffer(Buffer* vertexBuffer, uint32_t offset)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::Draw(uint32_t vertexCount)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void MetalCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
		{
		}
	}
}
