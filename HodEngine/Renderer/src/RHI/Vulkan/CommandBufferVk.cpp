
#include <ImGui/src/imgui.h>
#include <ImGui/src/imgui_impl_vulkan.h>

#include "CommandBufferVk.h"

#include "VkMaterialInstance.h"
#include "VkMaterial.h"
#include "BufferVk.h"

#include "RendererVulkan.h"

#include <Core/Src/Output.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		CommandBufferVk::CommandBufferVk()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = renderer->GetCommandPool();
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			if (vkAllocateCommandBuffers(renderer->GetVkDevice(), &allocInfo, &_vkCommandBuffer) != VK_SUCCESS)
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
			delete _sharedMinimalMaterialInstance;
			_sharedMinimalMaterialInstance = nullptr;

			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			vkFreeCommandBuffers(renderer->GetVkDevice(), renderer->GetCommandPool(), 1, &_vkCommandBuffer);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkCommandBuffer CommandBufferVk::GetVkCommandBuffer() const
		{
			return _vkCommandBuffer;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool CommandBufferVk::StartRecord()
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkMaterial* material = (VkMaterial*)renderer->GetSharedMinimalMaterial();
			_sharedMinimalMaterialInstance = (VkMaterialInstance*)renderer->CreateMaterialInstance(material);

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			beginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to begin recording command buffer!");
				return false;
			}

			VkClearValue clearColor[2];
			clearColor[0] = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearColor[1] = { 1.0f, 0.0f, 0.0f, 1.0f };

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderer->GetRenderPass();
			renderPassInfo.framebuffer = renderer->GetSwapChainCurrentFrameBuffer();
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = renderer->GetSwapChainExtent();
			renderPassInfo.clearValueCount = 2;
			renderPassInfo.pClearValues = clearColor;

			vkCmdBeginRenderPass(_vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = (float)renderPassInfo.renderArea.extent.height;
			viewport.width = (float)renderPassInfo.renderArea.extent.width;
			viewport.height = -(float)renderPassInfo.renderArea.extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(_vkCommandBuffer, 0, 1, &viewport);

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = renderPassInfo.renderArea.extent;

			vkCmdSetScissor(_vkCommandBuffer, 0, 1, &scissor);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool CommandBufferVk::EndRecord()
		{
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _vkCommandBuffer);

			vkCmdEndRenderPass(_vkCommandBuffer);

			if (vkEndCommandBuffer(_vkCommandBuffer) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to recording command buffer!");
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetProjectionMatrix(const glm::mat4x4& projectionMatrix)
		{
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.proj", projectionMatrix);
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.vp", projectionMatrix * _sharedMinimalMaterialInstance->GetMat4("viewUbo.view"));

			SetMaterialInstance(_sharedMinimalMaterialInstance, 0, 1);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetViewMatrix(const glm::mat4x4& viewMatrix)
		{
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.view", viewMatrix);
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.vp", _sharedMinimalMaterialInstance->GetMat4("viewUbo.proj") * viewMatrix);

			SetMaterialInstance(_sharedMinimalMaterialInstance, 0, 1);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetModelMatrix(const glm::mat4x4& modelMatrix)
		{
			MaterialInstance* modelMaterialInstance = Renderer::GetInstance()->CreateMaterialInstance(&_sharedMinimalMaterialInstance->GetMaterial());
			modelMaterialInstance->SetMat4("modelUbo.model", modelMatrix);
			modelMaterialInstance->SetMat4("modelUbo.mvp", _sharedMinimalMaterialInstance->GetMat4("viewUbo.vp") * modelMatrix);

			SetMaterialInstance(modelMaterialInstance, 1, 1);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetViewport(const CORE::Rect& viewport)
		{
			RendererVulkan* renderer = (RendererVulkan*)Renderer::GetInstance();

			VkExtent2D extent = renderer->GetSwapChainExtent();

			VkViewport vkViewport = {};
			vkViewport.x = viewport._position.x * extent.width; //0.0f;
			vkViewport.y = extent.height - (viewport._position.y * extent.height); //(float)_swapChainExtent.height;
			vkViewport.width = viewport._size.x * extent.width; // (float)_swapChainExtent.width;
			vkViewport.height = -viewport._size.y * extent.height; // -(float)_swapChainExtent.height;
			vkViewport.minDepth = 0.0f;
			vkViewport.maxDepth = 1.0f;

			vkCmdSetViewport(_vkCommandBuffer, 0, 1, &vkViewport);

			// Todo made dedicated command for scissor
			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = extent;

			vkCmdSetScissor(_vkCommandBuffer, 0, 1, &scissor);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetMaterialInstance(MaterialInstance* materialInstance, uint32_t setOffset, uint32_t setCount)
		{
			VkMaterialInstance* vkMaterialInstance = static_cast<VkMaterialInstance*>(materialInstance);

			const VkMaterial* material = static_cast<const VkMaterial*>(&vkMaterialInstance->GetMaterial());

			std::vector<VkDescriptorSet> descriptorSets = vkMaterialInstance->GetDescriptorSets(setOffset, setCount);

			vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetGraphicsPipeline());
			if (descriptorSets.empty() == false)
			{
				vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), setOffset, (uint32_t)descriptorSets.size(), descriptorSets.data(), 0, nullptr);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetVertexBuffer(Buffer* vertexBuffer)
		{
			VkBuffer vkBuffer = static_cast<BufferVk*>(vertexBuffer)->GetVkBuffer();
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(_vkCommandBuffer, 0, 1, &vkBuffer, offsets);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetIndexBuffer(Buffer* indexBuffer)
		{
			vkCmdBindIndexBuffer(_vkCommandBuffer, static_cast<BufferVk*>(indexBuffer)->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT16);
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
		void CommandBufferVk::DrawIndexed(uint32_t indexCount)
		{
			vkCmdDrawIndexed(_vkCommandBuffer, indexCount, 1, 0, 0, 0);
		}
	}
}
