#pragma once

#include "../CommandBuffer.h"

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

			void				SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType) override;

			void				SetProjectionMatrix(const glm::mat4x4& projectionMatrix) override;
			void				SetViewMatrix(const glm::mat4x4& viewMatrix) override;
			void				SetModelMatrix(const glm::mat4x4& modelMatrix) override;

			void				SetViewport(const CORE::Rect& viewport) override;
			void				SetScissor(const CORE::Rect& scissor) override;

			void				SetMaterialInstance(MaterialInstance* materialInstance, uint32_t setOffset = 2, uint32_t setCount = UINT32_MAX) override;
			void				SetVertexBuffer(Buffer* vertexBuffer, uint32_t offset = 0) override;
			void				SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) override;

			void				Draw(uint32_t vertexCount) override;
			void				DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) override;

		private:

			void				Release();

		private:

			const VkMaterial*	_material = nullptr;

			VkCommandBuffer		_vkCommandBuffer = VK_NULL_HANDLE;

			VkMaterialInstance* _sharedMinimalMaterialInstance = nullptr;
		};
	}
}
