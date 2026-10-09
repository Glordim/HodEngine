#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/CommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace hod::inline rhi
{
	class GraphicsPipelineVulkan;

	/// @brief
	class HOD_RHI_API CommandBufferVk : public CommandBuffer
	{
	public:
		CommandBufferVk();
		CommandBufferVk(const CommandBufferVk&) = delete;
		CommandBufferVk(CommandBufferVk&&) = delete;
		~CommandBufferVk() override;

		void operator=(const CommandBufferVk&) = delete;
		void operator=(CommandBufferVk&&) = delete;

	public:
		VkCommandBuffer GetVkCommandBuffer() const;

		bool StartRecord() override;
		bool EndRecord() override;

		bool StartRenderPass(RenderTarget* renderTarget = nullptr, PresentationSurface* presentationSurface = nullptr, const Color& color = Color(0.1f, 0.1f, 0.1f, 1.0f)) override;
		bool EndRenderPass() override;

		void SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType) override;

		void SetProjectionMatrix(const Matrix4& projectionMatrix) override;
		void SetViewMatrix(const Matrix4& viewMatrix) override;
		void SetModelMatrix(const Matrix4& modelMatrix) override;

		void SetViewport(const Rect& viewport) override;
		void SetScissor(const Rect& scissor) override;

		void SetGraphicsPipeline(const GraphicsPipeline* graphicsPipeline) override;
		void SetBindGroup(uint32_t set, const BindGroup* bindGroup, const uint32_t* uniformBufferOffsets, uint32_t uniformBufferOffsetCount) override;
		void SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset = 0) override;
		void SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) override;

		void Draw(uint32_t vertexCount) override;
		void DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) override;

		void Present(PresentationSurface* presentationSurface) override;

	private:
		void Release();

	private:
		const GraphicsPipelineVulkan* _graphicsPipeline = nullptr;

		VkCommandBuffer _vkCommandBuffer = VK_NULL_HANDLE;

		VkRenderPass _currentRenderPass = VK_NULL_HANDLE;

	};
}
