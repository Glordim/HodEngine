#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/CommandBufferVk.hpp"

#include "HodEngine/RHI/Vulkan/BindGroupVulkan.hpp"
#include "HodEngine/RHI/Vulkan/BufferVk.hpp"
#include "HodEngine/RHI/Vulkan/GraphicsPipelineVulkan.hpp"

#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"
#include "HodEngine/RHI/Vulkan/VkPresentationSurface.hpp"
#include "HodEngine/RHI/Vulkan/VkRenderTarget.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <stdlib.h>

#undef min

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	CommandBufferVk::CommandBufferVk()
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = rhiDevice->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(rhiDevice->GetVkDevice(), &allocInfo, &_vkCommandBuffer) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create Command Buffer!");
			return;
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	CommandBufferVk::~CommandBufferVk()
	{
		Release();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::Release()
	{
		RhiDeviceVulkan* rhiDevice = (RhiDeviceVulkan*)RhiDevice::GetInstance();

		vkFreeCommandBuffers(rhiDevice->GetVkDevice(), rhiDevice->GetCommandPool(), 1, &_vkCommandBuffer);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkCommandBuffer CommandBufferVk::GetVkCommandBuffer() const
	{
		return _vkCommandBuffer;
	}

	/// @brief
	/// @return
	bool CommandBufferVk::StartRecord()
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to begin recording command buffer!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	bool CommandBufferVk::EndRecord()
	{
		if (vkEndCommandBuffer(_vkCommandBuffer) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to recording command buffer!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @param renderTarget
	/// @param presentationSurface
	/// @param color
	bool CommandBufferVk::StartRenderPass(RenderTarget* renderTarget, PresentationSurface* presentationSurface, const Color& color)
	{
		VkClearValue clearColor[1];
		clearColor[0].color.float32[0] = color.r;
		clearColor[0].color.float32[1] = color.g;
		clearColor[0].color.float32[2] = color.b;
		clearColor[0].color.float32[3] = color.a;

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		if (renderTarget == nullptr)
		{
			VkPresentationSurface* vkPresentationSurface = (VkPresentationSurface*)presentationSurface;
			renderPassInfo.renderPass = vkPresentationSurface->GetRenderPass();
			renderPassInfo.framebuffer = vkPresentationSurface->GetSwapChainCurrentFrameBuffer();
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = vkPresentationSurface->GetSwapChainExtent();
		}
		else
		{
			VkRenderTarget* vkRenderTarget = static_cast<VkRenderTarget*>(renderTarget);

			renderPassInfo.renderPass = vkRenderTarget->GetRenderPass();
			renderPassInfo.framebuffer = vkRenderTarget->GetFrameBuffer();
			renderPassInfo.renderArea.offset = {0, 0};
			Vector2 resolution = vkRenderTarget->GetResolution();
			renderPassInfo.renderArea.extent.width = (uint32_t)resolution.GetX();
			renderPassInfo.renderArea.extent.height = (uint32_t)resolution.GetY();
		}
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = clearColor;

		_currentRenderPass = renderPassInfo.renderPass;

		vkCmdBeginRenderPass(_vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport vkViewport = {};
		vkViewport.x = 0;
		vkViewport.y = (float)renderPassInfo.renderArea.extent.height;
		vkViewport.width = (float)renderPassInfo.renderArea.extent.width;
		vkViewport.height = -(float)renderPassInfo.renderArea.extent.height;
		vkViewport.minDepth = 0.0f;
		vkViewport.maxDepth = 1.0f;

		vkCmdSetViewport(_vkCommandBuffer, 0, 1, &vkViewport);

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = renderPassInfo.renderArea.extent;

		vkCmdSetScissor(_vkCommandBuffer, 0, 1, &scissor);

		return true; // TODO cant fail
	}

	/// @brief
	bool CommandBufferVk::EndRenderPass()
	{
		vkCmdEndRenderPass(_vkCommandBuffer);
		_currentRenderPass = VK_NULL_HANDLE;
		return true; // TODO cant fail
	}

	/// @brief
	/// @param constant
	/// @param size
	/// @param shaderType
	void CommandBufferVk::SetConstant(void* constant, uint32_t size, Shader::ShaderType /*shaderType*/)
	{
		size = std::min(size, _graphicsPipeline->GetPushConstantSize());

		vkCmdPushConstants(_vkCommandBuffer, _graphicsPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, size, constant);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::SetProjectionMatrix(const Matrix4& projectionMatrix)
	{
		_projection = projectionMatrix;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::SetViewMatrix(const Matrix4& viewMatrix)
	{
		_view = viewMatrix;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::SetModelMatrix(const Matrix4& /*modelMatrix*/)
	{
	}

	/// @brief
	/// @param viewport
	void CommandBufferVk::SetViewport(const Rect& viewport)
	{
		VkViewport vkViewport = {};
		vkViewport.x = viewport._position.GetX();
		vkViewport.y = viewport._position.GetY() + viewport._size.GetY();
		vkViewport.width = viewport._size.GetX();
		vkViewport.height = -viewport._size.GetY();
		vkViewport.minDepth = 0.0f;
		vkViewport.maxDepth = 1.0f;

		vkCmdSetViewport(_vkCommandBuffer, 0, 1, &vkViewport);
	}

	/// @brief
	/// @param scissor
	void CommandBufferVk::SetScissor(const Rect& scissor)
	{
		VkRect2D vkScissor = {};
		vkScissor.offset = {std::max((int32_t)scissor._position.GetX(), 0), std::max((int32_t)scissor._position.GetY(), 0)};
		vkScissor.extent = {(uint32_t)scissor._size.GetX(), (uint32_t)scissor._size.GetY()};

		vkCmdSetScissor(_vkCommandBuffer, 0, 1, &vkScissor);
	}

	/// @brief
	/// @param graphicsPipeline
	void CommandBufferVk::SetGraphicsPipeline(const GraphicsPipeline* graphicsPipeline)
	{
		const GraphicsPipelineVulkan* graphicsPipelineVulkan = static_cast<const GraphicsPipelineVulkan*>(graphicsPipeline);
		if (_graphicsPipeline != graphicsPipelineVulkan)
		{
			_graphicsPipeline = graphicsPipelineVulkan;
			vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, const_cast<GraphicsPipelineVulkan*>(_graphicsPipeline)->GetVkPipeline(_currentRenderPass));
		}
	}

	/// @brief
	/// @param set
	/// @param bindGroup
	/// @param uniformBufferOffsets
	/// @param uniformBufferOffsetCount
	void CommandBufferVk::SetBindGroup(uint32_t set, const BindGroup* bindGroup, const uint32_t* uniformBufferOffsets, uint32_t uniformBufferOffsetCount)
	{
		Assert(_graphicsPipeline != nullptr); // the set is bound against the layout of the current pipeline

		const BindGroupVulkan*  bindGroupVulkan = static_cast<const BindGroupVulkan*>(bindGroup);
		const Vector<uint32_t>& uniformBufferOrder = bindGroupVulkan->GetUniformBufferOrder();
		Assert(uniformBufferOffsetCount == uniformBufferOrder.Size());

		uint32_t  dynamicOffsetCount = (uint32_t)uniformBufferOrder.Size();
		uint32_t* dynamicOffsets = (uint32_t*)alloca(sizeof(uint32_t) * (dynamicOffsetCount + 1));
		for (uint32_t index = 0; index < dynamicOffsetCount; ++index)
		{
			dynamicOffsets[index] = uniformBufferOffsets[uniformBufferOrder[index]];
		}

		VkDescriptorSet descriptorSet = bindGroupVulkan->GetDescriptorSet();
		vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->GetPipelineLayout(), set, 1, &descriptorSet, dynamicOffsetCount,
		                        dynamicOffsets);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset)
	{
		VkBuffer*     vkBuffers = (VkBuffer*)alloca(sizeof(VkBuffer) * count);
		VkDeviceSize* bufferOffsets = (VkDeviceSize*)alloca(sizeof(VkDeviceSize) * count);
		for (uint32_t index = 0; index < count; ++index)
		{
			vkBuffers[index] = static_cast<BufferVk*>(vertexBuffer[index])->GetVkBuffer();
			bufferOffsets[index] = offset;
		}

		vkCmdBindVertexBuffers(_vkCommandBuffer, 0, count, vkBuffers, bufferOffsets);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset)
	{
		vkCmdBindIndexBuffer(_vkCommandBuffer, static_cast<BufferVk*>(indexBuffer)->GetVkBuffer(), offset, VK_INDEX_TYPE_UINT16);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::Draw(uint32_t vertexCount)
	{
		vkCmdDraw(_vkCommandBuffer, vertexCount, 1, 0, 0);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBufferVk::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
	{
		vkCmdDrawIndexed(_vkCommandBuffer, indexCount, 1, indexOffset, vertexOffset, 0);
	}

	void CommandBufferVk::Present(PresentationSurface* /*presentationSurface*/) {}
}
