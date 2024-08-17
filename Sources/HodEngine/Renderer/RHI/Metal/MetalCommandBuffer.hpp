#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

namespace MTL
{
	class CommandBuffer;
	class RenderCommandEncoder;
}

namespace hod
{
	namespace renderer
	{
		class MetalBuffer;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API MetalCommandBuffer : public CommandBuffer
		{
		public:

								MetalCommandBuffer();
								MetalCommandBuffer(const MetalCommandBuffer&) = delete;
								MetalCommandBuffer(MetalCommandBuffer&&) = delete;
								~MetalCommandBuffer() override;

			void				operator=(const MetalCommandBuffer&) = delete;
			void				operator=(MetalCommandBuffer&&) = delete;

		public:

			bool				StartRecord(RenderTarget* renderTarget = nullptr, Context* context = nullptr) override;
			bool				EndRecord() override;

			void				SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType) override;

			void				SetProjectionMatrix(const Matrix4& projectionMatrix) override;
			void				SetViewMatrix(const Matrix4& viewMatrix) override;
			void				SetModelMatrix(const Matrix4& modelMatrix) override;

			void				SetViewport(const Rect& viewport) override;
			void				SetScissor(const Rect& scissor) override;

			void				SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset = 2, uint32_t setCount = UINT32_MAX) override;
			void				SetVertexBuffer(Buffer* vertexBuffer, uint32_t offset = 0) override;
			void				SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) override;

			void				Draw(uint32_t vertexCount) override;
			void				DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) override;

			void				Present(Context* context) override;

			MTL::CommandBuffer*	GetNativeCommandBuffer() const;

		private:

			MTL::CommandBuffer*	_commandBuffer = nullptr;
			MTL::RenderCommandEncoder*	_renderCommandEncoder = nullptr;
            MetalBuffer* _indexBuffer = nullptr;
            uint32_t _indexBufferOffset = 0;
		};
	}
}
