#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace hod
{
	namespace renderer
	{
		class VkMaterial;
		class VkMaterialInstance;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API CommandBufferVk : public CommandBuffer
		{
		public:

								CommandBufferVk();
								CommandBufferVk(const CommandBufferVk&) = delete;
								CommandBufferVk(CommandBufferVk&&) = delete;
								~CommandBufferVk() override;

			void				operator=(const CommandBufferVk&) = delete;
			void				operator=(CommandBufferVk&&) = delete;

		public:

			VkCommandBuffer		GetVkCommandBuffer() const;

			bool				StartRecord(RenderTarget* renderTarget = nullptr, Context* context = nullptr) override;
			bool				EndRecord() override;

			void				SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType) override;

			void				SetProjectionMatrix(const Matrix4& projectionMatrix) override;
			void				SetViewMatrix(const Matrix4& viewMatrix) override;
			void				SetModelMatrix(const Matrix4& modelMatrix) override;

			void				SetViewport(const Rect& viewport) override;
			void				SetScissor(const Rect& scissor) override;

			void				SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset = 2, uint32_t setCount = UINT32_MAX) override;
			void				SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset = 0) override;
			void				SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) override;

			void				Draw(uint32_t vertexCount) override;
			void				DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) override;

			void				Present(Context* context) override;

		private:

			void				Release();

		private:

			const VkMaterial*	_material = nullptr;

			VkCommandBuffer		_vkCommandBuffer = VK_NULL_HANDLE;

			//VkMaterialInstance* _sharedMinimalMaterialInstance = nullptr;
		};
	}
}
