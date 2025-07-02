#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/CommandBufferVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/VkMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterial.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkRenderTarget.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <stdlib.h>

#undef min

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
			//DefaultAllocator::GetInstance().Delete(_sharedMinimalMaterialInstance);
			//_sharedMinimalMaterialInstance = nullptr;

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
		/// @param context 
		/// @param color 
		bool CommandBufferVk::StartRenderPass(RenderTarget* renderTarget, Context* context, const Color& color)
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
				VkContext* vkContext = (VkContext*)context;
				if (vkContext == nullptr)
				{
					RendererVulkan* renderer = RendererVulkan::GetInstance();
					vkContext = renderer->GetContext();
				}

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
				Vector2 resolution = vkRenderTarget->GetResolution();
				renderPassInfo.renderArea.extent.width = (uint32_t)resolution.GetX();
				renderPassInfo.renderArea.extent.height = (uint32_t)resolution.GetY();
			}
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = clearColor;

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
			scissor.offset = { 0, 0 };
			scissor.extent = renderPassInfo.renderArea.extent;

			vkCmdSetScissor(_vkCommandBuffer, 0, 1, &scissor);

			return true; // TODO cant fail
		}

		/// @brief 
		bool CommandBufferVk::EndRenderPass()
		{
			vkCmdEndRenderPass(_vkCommandBuffer);
			return true; // TODO cant fail
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

			size = std::min(size, _material->GetPushConstantSize());

			vkCmdPushConstants(_vkCommandBuffer, _material->GetPipelineLayout(), shaderStage, 0, size, constant);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetProjectionMatrix(const Matrix4& projectionMatrix)
		{
			_projection = projectionMatrix;
			//_sharedMinimalMaterialInstance->SetMat4("viewUbo.proj", projectionMatrix);
			//_sharedMinimalMaterialInstance->SetMat4("viewUbo.vp", projectionMatrix * _sharedMinimalMaterialInstance->GetMat4("viewUbo.view"));

			//SetMaterialInstance(_sharedMinimalMaterialInstance, 0, 1);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetViewMatrix(const Matrix4& viewMatrix)
		{
			_view = viewMatrix;
			/*
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.view", viewMatrix);
			_sharedMinimalMaterialInstance->SetMat4("viewUbo.vp", _sharedMinimalMaterialInstance->GetMat4("viewUbo.proj") * viewMatrix);

			SetMaterialInstance(_sharedMinimalMaterialInstance, 0, 1);
			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetModelMatrix(const Matrix4& modelMatrix)
		{
			/*
			MaterialInstance* modelMaterialInstance = Renderer::GetInstance()->CreateMaterialInstance(&_sharedMinimalMaterialInstance->GetMaterial());
			modelMaterialInstance->SetMat4("modelUbo.model", modelMatrix);
			modelMaterialInstance->SetMat4("modelUbo.mvp", _sharedMinimalMaterialInstance->GetMat4("viewUbo.vp") * modelMatrix);

			SetMaterialInstance(modelMaterialInstance, 1, 1);
			*/
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
			vkScissor.offset = { std::max((int32_t)scissor._position.GetX(), 0), std::max((int32_t)scissor._position.GetY(), 0) };
			vkScissor.extent = { (uint32_t)scissor._size.GetX(), (uint32_t)scissor._size.GetY() };

			vkCmdSetScissor(_vkCommandBuffer, 0, 1, &vkScissor);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset, uint32_t setCount)
		{
			const VkMaterialInstance* vkMaterialInstance = static_cast<const VkMaterialInstance*>(materialInstance);

			const VkMaterial* material = static_cast<const VkMaterial*>(&vkMaterialInstance->GetMaterial());
			if (_material != material)
			{
				_material = material;
				vkCmdBindPipeline(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->GetGraphicsPipeline());
			}

			Vector<VkDescriptorSet> descriptorSets = vkMaterialInstance->GetDescriptorSets(setOffset, setCount);

			if (descriptorSets.empty() == false)
			{
				vkCmdBindDescriptorSets(_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->GetPipelineLayout(), setOffset, (uint32_t)descriptorSets.size(), descriptorSets.data(), 0, nullptr);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBufferVk::SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset)
		{
			VkBuffer* vkBuffers = (VkBuffer*)alloca(sizeof(VkBuffer) * count);
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

		void CommandBufferVk::Present(Context* context)
		{

		}
	}
}
