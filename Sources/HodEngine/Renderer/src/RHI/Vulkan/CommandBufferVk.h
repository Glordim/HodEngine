#pragma once

#include "../CommandBuffer.h"

#include <vulkan/vulkan.h>

namespace HOD
{
	namespace RENDERER
	{
		class VkMaterialInstance;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class CommandBufferVk : public CommandBuffer
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

			bool				StartRecord(Context* context = nullptr) override;
			bool				EndRecord() override;

			void				SetProjectionMatrix(const glm::mat4x4& projectionMatrix) override;
			void				SetViewMatrix(const glm::mat4x4& viewMatrix) override;
			void				SetModelMatrix(const glm::mat4x4& modelMatrix) override;

			void				SetViewport(const CORE::Rect& viewport) override;
			void				SetScissor(const CORE::Rect& scissor) override;

			void				SetMaterialInstance(MaterialInstance* materialInstance, uint32_t setOffset = 2, uint32_t setCount = UINT32_MAX) override;
			void				SetVertexBuffer(Buffer* vertexBuffer) override;
			void				SetIndexBuffer(Buffer* indexBuffer) override;

			void				Draw(uint32_t vertexCount) override;
			void				DrawIndexed(uint32_t indexCount) override;

		private:

			void				Release();

		private:

			VkCommandBuffer		_vkCommandBuffer = VK_NULL_HANDLE;

			VkMaterialInstance* _sharedMinimalMaterialInstance = nullptr;
		};
	}
}
