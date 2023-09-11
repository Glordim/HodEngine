
#include "HodEngine/Renderer/RHI/Vulkan/CommandBufferVk.h"

#include "HodEngine/Renderer/RHI/Vulkan/VkMaterialInstance.h"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterial.h"
#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.h"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.h"
#include "HodEngine/Renderer/RHI/Vulkan/VkRenderTarget.h"

#include <HodEngine/Core/Output.h>

namespace hod
{
	namespace renderer
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

		/// @brief 
		/// @return 
		bool CommandBufferVk::StartRecord(RenderTarget* renderTarget, Context* context)
		{
			RendererVulkan* renderer = RendererVulkan::GetInstance();

			VkContext* vkContext = (VkContext*)context;
			if (vkContext == nullptr)
			{
				vkContext = renderer->GetContext();
			}
			VkMaterial* material = nullptr;//(VkMaterial*)renderer->GetSharedMinimalMaterial(); // todo
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

			VkClearValue clearColor[1];
			clearColor[0] = { 0.1f, 0.1f, 0.1f, 1.0f };
			//clearColor[1] = { 1.0f, 0.0f, 0.0f, 1.0f };

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			if (renderTarget == nullptr)
			{
				renderPassInfo.renderPass = vkContext->GetRenderPass();
				renderPassInfo.framebuffer = vkContext->GetSwapChainCurrentFrameBuffer();
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = vkContext->GetSwapChainExtent();
			}
			else
			{
				VkRenderTarget* vkRenderTarget = static_cast<VkRenderTarget*>(renderTarget);

				renderPassInfo.renderPass = vkRenderTarget->GetRenderPass();
				renderPassInfo.framebuffer = vkRenderTarget->GetFrameBuffer();
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent.width = vkRenderTarget->GetWidth();
				renderPassInfo.renderArea.extent.height = vkRenderTarget->GetHeight();
			}
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = clearColor;

			vkCmdBeginRenderPass(_vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			
			VkViewport vkViewport = {};
			vkViewport.x = 0;
			vkViewport.y = 0;
			vkViewport.width = renderPassInfo.renderArea.extent.width;
			vkViewport.height = renderPassInfo.renderArea.extent.height;
			vkViewport.minDepth = 0.0f;
			vkViewport.maxDepth = 1.0f;

			vkCmdSetViewport(_vkCommandBuffer, 0, 1, &vkViewport);

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
			//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _vkCommandBuffer);

			vkCmdEndRenderPass(_vkCommandBuffer);

			if (vkEndCommandBuffer(_vkCommandBuffer) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to recording command buffer!");
				return false;
			}

			return true;
		}

		/// @brief 
		/// @param constant 
		/// @param size 
		/// @param shaderType 
		void CommandBufferVk::SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType)
		{
			VkShaderStageFlags shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
			if (shaderType == Shader::ShaderType::Fragment)
			{
				shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
			}

			vkCmdPushConstants(_vkCommandBuffer, _material->GetPipelineLayout(), shaderStage, 0, size, constant);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetProjectionMatrix(const Matrix4& projectionMatrix)
		{
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.proj", projectionMatrix);
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.vp", projectionMatrix * _sharedMinimalMaterialInstance->GetMat4("viewUbo.view"));

			SetMaterialInstance(_sharedMinimalMaterialInstance, 0, 1);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetViewMatrix(const Matrix4& viewMatrix)
		{
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.view", viewMatrix);
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.vp", _sharedMinimalMaterialInstance->GetMat4("viewUbo.proj") * viewMatrix);

			SetMaterialInstance(_sharedMinimalMaterialInstance, 0, 1);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetModelMatrix(const Matrix4& modelMatrix)
		{
			MaterialInstance* modelMaterialInstance = Renderer::GetInstance()->CreateMaterialInstance(&_sharedMinimalMaterialInstance->GetMaterial());
			modelMaterialInstance->SetMat4("modelUbo.model", modelMatrix);
			modelMaterialInstance->SetMat4("modelUbo.mvp", _sharedMinimalMaterialInstance->GetMat4("viewUbo.vp") * modelMatrix);

			SetMaterialInstance(modelMaterialInstance, 1, 1);
		}

		/// @brief 
		/// @param viewport 
		void CommandBufferVk::SetViewport(const Rect& viewport)
		{
			VkViewport vkViewport = {};
			vkViewport.x = viewport._position.GetX();
			vkViewport.y = viewport._position.GetY();
			vkViewport.width = viewport._size.GetX();
			vkViewport.height = viewport._size.GetY();
			vkViewport.minDepth = 0.0f;
			vkViewport.maxDepth = 1.0f;

			vkCmdSetViewport(_vkCommandBuffer, 0, 1, &vkViewport);
		}

		/// @brief 
		/// @param scissor 
		void CommandBufferVk::SetScissor(const Rect& scissor)
		{
			VkRect2D vkScissor = {};
			vkScissor.offset = { (int32_t)scissor._position.GetX(), (int32_t)scissor._position.GetY() };
			vkScissor.extent = { (uint32_t)scissor._size.GetX(), (uint32_t)scissor._size.GetY() };

			vkCmdSetScissor(_vkCommandBuffer, 0, 1, &vkScissor);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetMaterialInstance(MaterialInstance* materialInstance, uint32_t setOffset, uint32_t setCount)
		{
			VkMaterialInstance* vkMaterialInstance = static_cast<VkMaterialInstance*>(materialInstance);

			const VkMaterial* material = static_cast<const VkMaterial*>(&vkMaterialInstance->GetMaterial());
			if (_material != material)
			{
				_material = material;
				vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->GetGraphicsPipeline());
			}

			std::vector<VkDescriptorSet> descriptorSets = vkMaterialInstance->GetDescriptorSets(setOffset, setCount);

			if (descriptorSets.empty() == false)
			{
				vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->GetPipelineLayout(), setOffset, (uint32_t)descriptorSets.size(), descriptorSets.data(), 0, nullptr);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetVertexBuffer(Buffer* vertexBuffer, uint32_t offset)
		{
			VkBuffer vkBuffer = static_cast<BufferVk*>(vertexBuffer)->GetVkBuffer();
			VkDeviceSize offsets[] = { offset };

			vkCmdBindVertexBuffers(_vkCommandBuffer, 0, 1, &vkBuffer, offsets);
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
	}
}
