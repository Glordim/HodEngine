#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"

#include "HodEngine/RHI/Vulkan/VulkanBindGroup.hpp"
#include "HodEngine/RHI/Vulkan/VulkanBuffer.hpp"
#include "HodEngine/RHI/Vulkan/VulkanCommandBuffer.hpp"
#include "HodEngine/RHI/Vulkan/VulkanFence.hpp"
#include "HodEngine/RHI/Vulkan/VulkanSemaphore.hpp"
#include "HodEngine/RHI/Vulkan/VulkanGraphicsPipeline.hpp"
#include "HodEngine/RHI/Vulkan/VulkanPresentationSurface.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRenderTarget.hpp"
#include "HodEngine/RHI/Vulkan/VulkanSampler.hpp"
#include "HodEngine/RHI/Vulkan/VulkanShader.hpp"
#include "HodEngine/RHI/Vulkan/VulkanTexture.hpp"

#include "HodEngine/RHI/Vulkan/ExtensionCollector/DeviceExtensionCollector.hpp"
#include "HodEngine/RHI/Vulkan/ExtensionCollector/InstanceExtensionCollector.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Window/Window.hpp>

#undef min
#undef max


#if defined(PLATFORM_WINDOWS)
	#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp>
	#include <vulkan/vulkan_win32.h>
#elif defined(PLATFORM_LINUX)
	#include <HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.hpp>
	#include <HodEngine/Window/Desktop/Linux/Wayland/WaylandWindow.hpp>
	#include <vulkan/vulkan_wayland.h>
#elif defined(PLATFORM_ANDROID)
	#include <HodEngine/Window/Android/AndroidWindow.hpp>
	#include <vulkan/vulkan_android.h>
#endif

namespace hod::inline rhi
{
	namespace
	{
		/// @brief Destroys, in the order they were released, the objects the GPU is done with: the first ones, as their submission indices only grow
		template<typename Deferred, typename Destroy>
		void ReleaseCompleted(Vector<Deferred>& objects, uint64_t completedSubmissionIndex, Destroy destroy)
		{
			uint32_t completedCount = 0;
			while (completedCount < objects.Size() && objects[completedCount]._submissionIndex <= completedSubmissionIndex)
			{
				destroy(objects[completedCount]._object);
				++completedCount;
			}

			// The ones left are shifted to the front. A ring buffer would suit these lists better: they are only pushed to the back
			// and released from the front. Cheap for now, as only the ones still in flight are shifted, once per frame.
			if (completedCount > 0)
			{
				for (uint32_t index = completedCount; index < objects.Size(); ++index)
				{
					objects[index - completedCount] = objects[index];
				}
				objects.Resize(objects.Size() - completedCount);
			}
		}
	}

	_SingletonOverrideConstructor(VulkanRhiDevice)
	: RhiDevice()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VulkanRhiDevice::~VulkanRhiDevice()
	{
		WaitIdle();

		// Nothing is submitted anymore: what was released after the last submission can go too
		_completedSubmissionIndex = _lastSubmissionIndex + 1;
		ReleaseCompletedDeletions();

		for (const SubmittedFence& submittedFence : _submittedFences)
		{
			vkDestroyFence(_device, submittedFence._fence, nullptr);
		}
		_submittedFences.Clear();
		for (VkFence fence : _freeFences)
		{
			vkDestroyFence(_device, fence, nullptr);
		}
		_freeFences.Clear();

		for (const CompatibleRenderPass& compatibleRenderPass : _compatibleRenderPasses)
		{
			vkDestroyRenderPass(_device, compatibleRenderPass._renderPass, nullptr);
		}
		_compatibleRenderPasses.Clear();

		vmaDestroyAllocator(_vmaAllocator);

		for (VkDescriptorPool descriptorPool : _descriptorPools)
		{
			vkDestroyDescriptorPool(_device, descriptorPool, nullptr);
		}
		_descriptorPools.Clear();

		if (_commandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(_device, _commandPool, nullptr);
		}

		if (_device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(_device, nullptr);
		}

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		_validationLayer.DestroyMessager();
#endif

		if (_vkInstance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(_vkInstance, nullptr);
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void VulkanRhiDevice::WaitIdle()
	{
		if (_device != VK_NULL_HANDLE)
		{
			FlushUploads();

			if (vkDeviceWaitIdle(_device) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: DeviceWaitIdle failed!");
			}

			UpdateCompletedSubmissionIndex();
			ReleaseCompletedDeletions();
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkInstance VulkanRhiDevice::GetVkInstance() const
	{
		return _vkInstance;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkDevice VulkanRhiDevice::GetVkDevice() const
	{
		return _device;
	}

	/// @brief
	/// @return
	const VulkanGpuDevice* VulkanRhiDevice::GetVkGpuDevice() const
	{
		return _selectedGpu;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	/*
	VkRenderPass VulkanRhiDevice::GetRenderPass() const
	{
	    return _renderPass;
	}
	*/

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	/*
	VkExtent2D VulkanRhiDevice::GetSwapChainExtent() const
	{
	    return _swapChainExtent;
	}
	*/

	/// @brief
	/// @param descriptorSetLayout
	/// @param descriptorSet
	/// @param descriptorPool the pool it comes from
	/// @return
	bool VulkanRhiDevice::AllocateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet* descriptorSet, VkDescriptorPool* descriptorPool)
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		// The most recent pool first: the previous ones were full when it was created, until sets are freed from them
		for (uint32_t index = (uint32_t)_descriptorPools.Size(); index > 0; --index)
		{
			allocInfo.descriptorPool = _descriptorPools[index - 1];

			VkResult result = vkAllocateDescriptorSets(_device, &allocInfo, descriptorSet);
			if (result == VK_SUCCESS)
			{
				*descriptorPool = allocInfo.descriptorPool;
				return true;
			}
			if (result != VK_ERROR_OUT_OF_POOL_MEMORY && result != VK_ERROR_FRAGMENTED_POOL)
			{
				OUTPUT_ERROR("Vulkan: Unable to allocate a descriptor set! (VkResult: {})", (int32_t)result);
				return false;
			}
		}

		if (CreateDescriptorPool() == false)
		{
			return false;
		}

		allocInfo.descriptorPool = _descriptorPools.Back();
		VkResult result = vkAllocateDescriptorSets(_device, &allocInfo, descriptorSet);
		if (result != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to allocate a descriptor set! (VkResult: {})", (int32_t)result);
			return false;
		}

		*descriptorPool = allocInfo.descriptorPool;
		return true;
	}

	/// @brief A pool more, for when the others are full
	/// @return
	bool VulkanRhiDevice::CreateDescriptorPool()
	{
		// Room for the BindGroups of most scenes in one pool. They are sized by what a BindGroup holds: uniform buffers, textures and their samplers.
		constexpr uint32_t SetCount = 1024;

		VkDescriptorPoolSize poolSizes[4];

		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[0].descriptorCount = SetCount * 2;

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = SetCount;

		poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes[2].descriptorCount = SetCount;

		poolSizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes[3].descriptorCount = SetCount;

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = 4;
		descriptorPoolInfo.pPoolSizes = poolSizes;
		descriptorPoolInfo.maxSets = SetCount;
		descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		if (vkCreateDescriptorPool(_device, &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create descriptor pool!");
			return false;
		}

		_descriptorPools.PushBack(descriptorPool);
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	VkCommandPool VulkanRhiDevice::GetCommandPool() const
	{
		return _commandPool;
	}

	/// @brief
	/// @param colorFormat
	/// @param depthFormat
	/// @param colorFinalLayout
	/// @param renderPass
	/// @return
	bool VulkanRhiDevice::CreateRenderPass(PixelFormat colorFormat, PixelFormat depthFormat, VkImageLayout colorFinalLayout, VkRenderPass* renderPass)
	{
		*renderPass = VK_NULL_HANDLE;

		VkAttachmentDescription attachments[2] = {};
		uint32_t                attachmentCount = 0;

		VkAttachmentReference colorAttachmentRef = {};
		VkAttachmentReference depthAttachmentRef = {};

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		if (colorFormat != PixelFormat::None)
		{
			VkAttachmentDescription& colorAttachment = attachments[attachmentCount];
			colorAttachment.format = VulkanTexture::ToVkFormat(colorFormat);
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = colorFinalLayout;

			colorAttachmentRef.attachment = attachmentCount;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			++attachmentCount;
		}

		// Not kept after the render pass
		if (depthFormat != PixelFormat::None)
		{
			VkAttachmentDescription& depthAttachment = attachments[attachmentCount];
			depthAttachment.format = VulkanTexture::ToVkFormat(depthFormat);
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			depthAttachmentRef.attachment = attachmentCount;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;
			++attachmentCount;
		}

		// The same in every render pass: unlike the layouts and the load and store operations, the dependencies take part in the compatibility
		VkSubpassDependency dependencies[2] = {};

		// What drew into the attachments, or read them, before they are cleared
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
		                                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].srcAccessMask = 0;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		// What reads the color in a shader afterwards, when it is a render target
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachmentCount;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 2;
		renderPassInfo.pDependencies = dependencies;

		if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, renderPass) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create render pass!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @param colorFormat
	/// @param depthFormat
	/// @return
	VkRenderPass VulkanRhiDevice::GetCompatibleRenderPass(PixelFormat colorFormat, PixelFormat depthFormat)
	{
		for (const CompatibleRenderPass& compatibleRenderPass : _compatibleRenderPasses)
		{
			if (compatibleRenderPass._colorFormat == colorFormat && compatibleRenderPass._depthFormat == depthFormat)
			{
				return compatibleRenderPass._renderPass;
			}
		}

		// The final layout does not take part in the compatibility
		CompatibleRenderPass compatibleRenderPass;
		compatibleRenderPass._colorFormat = colorFormat;
		compatibleRenderPass._depthFormat = depthFormat;
		if (CreateRenderPass(colorFormat, depthFormat, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &compatibleRenderPass._renderPass) == false)
		{
			return VK_NULL_HANDLE;
		}
		_compatibleRenderPasses.PushBack(compatibleRenderPass);
		return compatibleRenderPass._renderPass;
	}

	/*
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	GraphicsPipeline* VulkanRhiDevice::GetSharedMinimalMaterial() const
	{
	    return _sharedMinimalMaterial;
	}
	*/

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkFramebuffer framebuffer)
	{
		_framebuffersToDestroy.PushBack({framebuffer, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkRenderPass renderPass)
	{
		_renderPassesToDestroy.PushBack({renderPass, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkSemaphore semaphore)
	{
		_vkSemaphoresToDestroy.PushBack({semaphore, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkSampler sampler)
	{
		_samplersToDestroy.PushBack({sampler, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkImageView imageView)
	{
		_imageViewsToDestroy.PushBack({imageView, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkImage image, VmaAllocation allocation)
	{
		_imagesToDestroy.PushBack({{image, allocation}, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkBuffer buffer, VmaAllocation allocation)
	{
		_buffersToDestroy.PushBack({{buffer, allocation}, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkDescriptorSet descriptorSet, VkDescriptorPool descriptorPool)
	{
		_descriptorSetsToDestroy.PushBack({{descriptorSet, descriptorPool}, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkPipeline pipeline)
	{
		_pipelinesToDestroy.PushBack({pipeline, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkPipelineLayout pipelineLayout)
	{
		_pipelineLayoutsToDestroy.PushBack({pipelineLayout, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	void VulkanRhiDevice::DeferDestroy(VkCommandBuffer commandBuffer)
	{
		_commandBuffersToDestroy.PushBack({commandBuffer, GetDeferredDeletionSubmissionIndex()});
	}

	/// @brief
	/// @return
	uint64_t VulkanRhiDevice::GetDeferredDeletionSubmissionIndex() const
	{
		return _lastSubmissionIndex + 1;
	}

	/// @brief
	/// @return
	bool VulkanRhiDevice::SignalSubmissionIndex()
	{
		VkFence fence = VK_NULL_HANDLE;
		if (_freeFences.Empty() == false)
		{
			fence = _freeFences.Back();
			_freeFences.PopBack();
		}
		else
		{
			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create a submission fence!");
				return false;
			}
		}

		// Without command buffer, signaled once the GPU is done with what was submitted before
		if (vkQueueSubmit(_graphicsQueue, 0, nullptr, fence) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to submit a submission fence!");
			_freeFences.PushBack(fence);
			return false;
		}

		++_lastSubmissionIndex;
		_submittedFences.PushBack({fence, _lastSubmissionIndex});
		return true;
	}

	/// @brief
	void VulkanRhiDevice::UpdateCompletedSubmissionIndex()
	{
		while (_submittedFences.Empty() == false && vkGetFenceStatus(_device, _submittedFences[0]._fence) == VK_SUCCESS)
		{
			SubmittedFence submittedFence = _submittedFences[0];
			_submittedFences.Erase(_submittedFences.Begin());

			_completedSubmissionIndex = submittedFence._submissionIndex;
			vkResetFences(_device, 1, &submittedFence._fence);
			_freeFences.PushBack(submittedFence._fence);
		}
	}

	/// @brief
	/// @return null if it can't be recorded
	VkCommandBuffer VulkanRhiDevice::GetUploadCommandBuffer()
	{
		if (_uploadCommandBuffer != VK_NULL_HANDLE)
		{
			return _uploadCommandBuffer;
		}

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		if (vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create the upload command buffer!");
			return VK_NULL_HANDLE;
		}

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to begin the upload command buffer!");
			vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
			return VK_NULL_HANDLE;
		}

		_uploadCommandBuffer = commandBuffer;
		return _uploadCommandBuffer;
	}

	/// @brief
	/// @param size
	/// @param buffer
	/// @param mappedData
	/// @return
	bool VulkanRhiDevice::CreateUploadBuffer(VkDeviceSize size, VkBuffer* buffer, void** mappedData)
	{
		// Released once the next submission is done, the one that carries the upload commands
		if (GetUploadCommandBuffer() == VK_NULL_HANDLE)
		{
			return false;
		}

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; // host coherent
		allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocation     allocation = VK_NULL_HANDLE;
		VmaAllocationInfo allocationInfo = {};
		if (vmaCreateBuffer(_vmaAllocator, &bufferInfo, &allocationCreateInfo, buffer, &allocation, &allocationInfo) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create an upload buffer!");
			return false;
		}

		*mappedData = allocationInfo.pMappedData;
		DeferDestroy(*buffer, allocation);
		return true;
	}

	/// @brief
	/// @return
	VkCommandBuffer VulkanRhiDevice::EndUploads()
	{
		if (_uploadCommandBuffer == VK_NULL_HANDLE)
		{
			return VK_NULL_HANDLE;
		}

		VkCommandBuffer commandBuffer = _uploadCommandBuffer;
		_uploadCommandBuffer = VK_NULL_HANDLE;
		DeferDestroy(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to end the upload command buffer!");
			return VK_NULL_HANDLE;
		}
		return commandBuffer;
	}

	/// @brief
	void VulkanRhiDevice::FlushUploads()
	{
		VkCommandBuffer commandBuffer = EndUploads();
		if (commandBuffer == VK_NULL_HANDLE)
		{
			return;
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to submit the upload command buffer!");
			return;
		}
		SignalSubmissionIndex();
	}

	/// @brief Whatever the frame: what is released is tagged with a submission index, not with a frame
	void VulkanRhiDevice::FlushDeferredDeletions(uint32_t /*frameIndex*/)
	{
		UpdateCompletedSubmissionIndex();
		ReleaseCompletedDeletions();
	}

	/// @brief
	void VulkanRhiDevice::ReleaseCompletedDeletions()
	{
		// What uses another object goes before it
		ReleaseCompleted(_framebuffersToDestroy, _completedSubmissionIndex, [this](VkFramebuffer framebuffer) { vkDestroyFramebuffer(_device, framebuffer, nullptr); });
		ReleaseCompleted(_vkSemaphoresToDestroy, _completedSubmissionIndex, [this](VkSemaphore semaphore) { vkDestroySemaphore(_device, semaphore, nullptr); });
		ReleaseCompleted(_samplersToDestroy, _completedSubmissionIndex, [this](VkSampler sampler) { vkDestroySampler(_device, sampler, nullptr); });
		ReleaseCompleted(_imageViewsToDestroy, _completedSubmissionIndex, [this](VkImageView imageView) { vkDestroyImageView(_device, imageView, nullptr); });
		ReleaseCompleted(_imagesToDestroy, _completedSubmissionIndex,
		                 [this](const DeferredImage& image)
		                 {
			                 vkDestroyImage(_device, image.image, nullptr);
			                 vmaFreeMemory(_vmaAllocator, image.allocation);
		                 });
		ReleaseCompleted(_buffersToDestroy, _completedSubmissionIndex, [this](const DeferredBuffer& buffer) { vmaDestroyBuffer(_vmaAllocator, buffer.buffer, buffer.allocation); });
		ReleaseCompleted(_descriptorSetsToDestroy, _completedSubmissionIndex, [this](const DeferredDescriptorSet& descriptorSet)
		                 { vkFreeDescriptorSets(_device, descriptorSet.descriptorPool, 1, &descriptorSet.descriptorSet); });
		ReleaseCompleted(_pipelinesToDestroy, _completedSubmissionIndex, [this](VkPipeline pipeline) { vkDestroyPipeline(_device, pipeline, nullptr); });
		ReleaseCompleted(_commandBuffersToDestroy, _completedSubmissionIndex, [this](VkCommandBuffer commandBuffer) { vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer); });
		ReleaseCompleted(_pipelineLayoutsToDestroy, _completedSubmissionIndex, [this](VkPipelineLayout pipelineLayout) { vkDestroyPipelineLayout(_device, pipelineLayout, nullptr); });
		ReleaseCompleted(_renderPassesToDestroy, _completedSubmissionIndex, [this](VkRenderPass renderPass) { vkDestroyRenderPass(_device, renderPass, nullptr); });
	}

	PresentationSurface* VulkanRhiDevice::CreatePresentationSurface(window::Window* window)
	{
		VulkanPresentationSurface* presentationSurface = DefaultAllocator::GetInstance().New<VulkanPresentationSurface>(window);
		presentationSurface->Resize(window->GetWidth(), window->GetHeight());
		_presentationSurfaces.PushBack(presentationSurface);
		return presentationSurface;
	}

	/// @brief
	/// @return
	bool VulkanRhiDevice::InitDevice(uint32_t physicalDeviceIdentifier)
	{

		if (CreateVkIntance() == false)
		{
			return false;
		}

		EnumeratePhysicalDevice();

		if (SelectPhysicalDevice(physicalDeviceIdentifier) == false)
		{
			return false;
		}

		if (CreateDevice() == false)
		{
			return false;
		}

		if (CreateCommandPool() == false || CreateDescriptorPool() == false)
		{
			return false;
		}

		VmaAllocatorCreateInfo vmaAllocatorInfo = {};
		vmaAllocatorInfo.physicalDevice = _selectedGpu->physicalDevice;
		vmaAllocatorInfo.device = _device;
		vmaAllocatorInfo.instance = _vkInstance;
		vmaAllocatorInfo.vulkanApiVersion = VK_API_VERSION_1_1;
		if (vmaCreateAllocator(&vmaAllocatorInfo, &_vmaAllocator) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	/// @brief
	/// @return
	bool VulkanRhiDevice::CreateVkIntance()
	{
		// === Extensions ===

		InstanceExtensionCollector instanceExtensionCollector;
		if (instanceExtensionCollector.CollectAvailableExtension() == false)
		{
			return false;
		}

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		if (_validationLayer.IsEnabled())
		{
			if (instanceExtensionCollector.CollectAvailableExtension("VK_LAYER_KHRONOS_validation") == false)
			{
				return false;
			}
			if (_validationLayer.CollectInstanceExtensionRequirements(instanceExtensionCollector) == false)
			{
				return false;
			}
		}
#endif

		if (VulkanPresentationSurface::CollectInstanceExtensionRequirements(instanceExtensionCollector) == false)
		{
			return false;
		}

		// === Create Instance ===

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello";                    // TODO ?
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // TODO ?
		appInfo.pEngineName = "HodEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // TODO ?
		appInfo.apiVersion = VK_API_VERSION_1_1;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionCollector.GetEnabledExtensions().Size());
		createInfo.ppEnabledExtensionNames = instanceExtensionCollector.GetEnabledExtensions().Data();
#if defined(RHI_ENABLE_VALIDATION_LAYER)
		createInfo.enabledLayerCount = _validationLayer.GetEnabledLayers().Size();
		createInfo.ppEnabledLayerNames = _validationLayer.GetEnabledLayers().Data();
#else
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#endif
		createInfo.pNext = instanceExtensionCollector.GetFirstNextFeature();

		OUTPUT_MESSAGE("Vulkan: Creating instance with {} extension(s), {} layer(s):", createInfo.enabledExtensionCount, createInfo.enabledLayerCount);
		for (uint32_t i = 0; i < createInfo.enabledExtensionCount; ++i)
		{
			OUTPUT_MESSAGE("  Extension: {}", createInfo.ppEnabledExtensionNames[i]);
		}
		for (uint32_t i = 0; i < createInfo.enabledLayerCount; ++i)
		{
			OUTPUT_MESSAGE("  Layer: {}", createInfo.ppEnabledLayerNames[i]);
		}

		VkResult vkResult = vkCreateInstance(&createInfo, nullptr, &_vkInstance);
		if (vkResult != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create a Vulkan instance ! (VkResult: {})", (int32_t)vkResult);
			return false;
		}

#if defined(RHI_ENABLE_VALIDATION_LAYER)
		if (_validationLayer.IsEnabled())
		{
			_validationLayer.CreateMessager();
		}
#endif

		return true;
	}

	/// @brief
	/// @param availableDevices
	/// @return
	bool VulkanRhiDevice::GetAvailableGpuDevices(Vector<GpuDevice*>* /*availableDevices*/)
	{
		return false; // TODO
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void VulkanRhiDevice::EnumeratePhysicalDevice()
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, nullptr);

		Vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, physicalDevices.Data());

		_availableGpu.Resize(physicalDeviceCount);

		_recommandedGpu = nullptr;
		for (size_t i = 0; i < physicalDeviceCount; ++i)
		{
			VkPhysicalDevice physicalDevice = physicalDevices[i];
			VulkanGpuDevice&     gpuDevice = _availableGpu[i];

			FillPhysicalDeviceInfo(physicalDevice, gpuDevice);

			if (gpuDevice.compatible == true)
			{
				if (_recommandedGpu == nullptr)
				{
					_recommandedGpu = &gpuDevice;
				}
				else if (gpuDevice.score > _recommandedGpu->score)
				{
					_recommandedGpu = &gpuDevice;
				}
			}
		}
	}

	/// @brief
	/// @param physicalDevice
	/// @param gpuDevice
	/// @return
	void VulkanRhiDevice::FillPhysicalDeviceInfo(VkPhysicalDevice physicalDevice, VulkanGpuDevice& gpuDevice)
	{
		gpuDevice.physicalDevice = physicalDevice;
		gpuDevice.compatible = false;
		gpuDevice.score = 0;

		vkGetPhysicalDeviceProperties(physicalDevice, &gpuDevice.deviceProperties);

		gpuDevice.name = std::wstring(gpuDevice.deviceProperties.deviceName, gpuDevice.deviceProperties.deviceName + strlen(gpuDevice.deviceProperties.deviceName));
		gpuDevice.score += gpuDevice.deviceProperties.limits.maxImageDimension2D;

		if (gpuDevice.deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			gpuDevice.score += 1000000;
		}

		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &gpuDevice.memProperties);

		size_t vram = 0;

		for (size_t i = 0; i < gpuDevice.memProperties.memoryHeapCount; ++i)
		{
			const VkMemoryHeap& heap = gpuDevice.memProperties.memoryHeaps[i];

			if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				if (heap.size > vram)
				{
					vram = heap.size / 1000000;
				}
			}
		}

		gpuDevice.vram = vram;
		gpuDevice.score += vram;

		gpuDevice.graphicsAndPresentQueueFamilyIndex = 0;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.Data());

		gpuDevice.compatible = false;
		for (size_t i = 0; i < queueFamilyCount; ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 || (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				gpuDevice.compatible = true;
				gpuDevice.graphicsAndPresentQueueFamilyIndex = i;
				break;
			}
		}

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		if (deviceFeatures.geometryShader == VK_FALSE)
		{
			gpuDevice.compatible = false;
			return;
		}
	}

	/// @brief
	/// @param physicalDeviceIdentifier
	/// @return
	bool VulkanRhiDevice::SelectPhysicalDevice(uint32_t physicalDeviceIdentifier)
	{
		_selectedGpu = nullptr;

		if (physicalDeviceIdentifier != 0)
		{
			for (const VulkanGpuDevice& gpu : _availableGpu)
			{
				if (gpu.deviceProperties.deviceID == physicalDeviceIdentifier)
				{
					_selectedGpu = &gpu;
					break;
				}
			}
			if (_selectedGpu == nullptr)
			{
				OUTPUT_ERROR("Vulkan: PhysicalDevice {} not found, fallback on recommanded physical device", physicalDeviceIdentifier);
				_selectedGpu = _recommandedGpu;
			}
			else if (_selectedGpu->compatible == false)
			{
				OUTPUT_ERROR("Vulkan: Selected PhysicalDevice {} is not compatible with GameEngine requirement, fallback on recommanded physical device", physicalDeviceIdentifier);
				_selectedGpu = _recommandedGpu;
			}
		}
		else
		{
			_selectedGpu = _recommandedGpu;
		}

		if (_selectedGpu == nullptr)
		{
			OUTPUT_ERROR("Vulkan: No PhysicalDevice compatible with GameEngine requirement... sorry");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VulkanRhiDevice::CreateDevice()
	{
		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = _selectedGpu->graphicsAndPresentQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.fillModeNonSolid = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		// Deprecated, now _device share Validation Layer with the VkInstance
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledLayerCount = 0;

		DeviceExtensionCollector deviceExtensionCollector;
		deviceExtensionCollector.CollectAvailableExtension(_selectedGpu->physicalDevice);

		if (VulkanPresentationSurface::CollectDeviceExtensionRequirements(deviceExtensionCollector) == false)
		{
			return false;
		}

		createInfo.enabledExtensionCount = (uint32_t)deviceExtensionCollector.GetEnabledExtensions().Size();
		createInfo.ppEnabledExtensionNames = deviceExtensionCollector.GetEnabledExtensions().Data();
		createInfo.pNext = deviceExtensionCollector.GetFirstNextFeature();

		if (vkCreateDevice(_selectedGpu->physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create logical device !");
			return false;
		}

		vkGetDeviceQueue(_device, _selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, _selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &_presentQueue);

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VulkanRhiDevice::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = _selectedGpu->graphicsAndPresentQueueFamilyIndex;
		poolInfo.flags = 0; // Optional

		if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create command pool!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VulkanRhiDevice::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
	                                 VkImage* image, VmaAllocation* imageMemory, uint32_t mipLevelCount)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevelCount;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = (properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ? VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_TO_GPU;

		if (vmaCreateImage(_vmaAllocator, &imageInfo, &allocInfo, image, imageMemory, nullptr) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create image!");
			return false;
		}
		// vmaBindImageMemory(_vmaAllocator, *imageMemory, *image);
		/*
		if (vkCreateImage(_device, &imageInfo, nullptr, image) != VK_SUCCESS)
		{
		    OUTPUT_ERROR("Vulkan: Unable to create image!");
		    return false;
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, *image, &memRequirements);

		uint32_t memoryTypeIndex = 0;

		if (FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties, &memoryTypeIndex) == false)
		{
		    return false;
		}

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(_device, &allocInfo, nullptr, imageMemory) != VK_SUCCESS)
		{
		    OUTPUT_ERROR("Vulkan: Unable to allocate image memory!");
		    return false;
		}

		if (vkBindImageMemory(_device, *image, *imageMemory, 0) != VK_SUCCESS)
		{
		    OUTPUT_ERROR("Vulkan: Unable to bind image and image memory!");
		    return false;
		}
		*/

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VulkanRhiDevice::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView, uint32_t mipLevelCount)
	{
		*imageView = VK_NULL_HANDLE;

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevelCount;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_device, &viewInfo, nullptr, imageView) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Failed to create texture image view!");
			return false;
		}

		return true;
	}

	/// @brief
	/// @param commandBuffer
	/// @param image
	/// @param aspectFlags
	/// @param oldLayout
	/// @param newLayout
	/// @return
	/// @param mipLevelCount
	bool VulkanRhiDevice::TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout,
	                                            uint32_t mipLevelCount)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevelCount;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else
		{
			OUTPUT_ERROR("Vulkan: TransitionImageLayout, unsupported layout transition!");
			return false;
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void VulkanRhiDevice::CopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage image, VkBuffer buffer, uint32_t width, uint32_t height)
	{
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = {width, height, 1};

		vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool VulkanRhiDevice::FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex)
	{
		for (uint32_t i = 0; i < _selectedGpu->memProperties.memoryTypeCount; ++i)
		{
			if ((memoryTypeBits & (1 << i)) && (_selectedGpu->memProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
			{
				*memoryTypeIndex = i;
				return true;
			}
		}

		return false;
	}

	/// @brief
	/// @param commandBuffers
	/// @param commandBufferCount
	/// @param signalSemaphore
	/// @param waitSemaphore
	/// @param fence
	/// @return
	bool VulkanRhiDevice::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore,
	                                          const Fence* fence)
	{
		Vector<VkCommandBuffer> vkCommandBuffers;
		vkCommandBuffers.Reserve(commandBufferCount + 1);

		// Ahead of what draws with what they upload: their barriers order them before it, as it comes after them on the queue
		VkCommandBuffer uploadCommandBuffer = EndUploads();
		if (uploadCommandBuffer != VK_NULL_HANDLE)
		{
			vkCommandBuffers.PushBack(uploadCommandBuffer);
		}

		for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
		{
			vkCommandBuffers.PushBack(static_cast<VulkanCommandBuffer*>(commandBuffers[commandBufferIndex])->GetVkCommandBuffer());
		}

		VkSemaphore vkSignalSemaphore = VK_NULL_HANDLE;
		VkSemaphore vkWaitSemaphore = VK_NULL_HANDLE;
		VkFence     vkFence = VK_NULL_HANDLE;
		if (fence != nullptr)
		{
			vkFence = static_cast<const VulkanFence*>(fence)->GetVkFence();
		}

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		if (signalSemaphore != nullptr)
		{
			vkSignalSemaphore = static_cast<const VulkanSemaphore*>(signalSemaphore)->GetVkSemaphore();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &vkSignalSemaphore;
		}
		else
		{
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
		}
		if (waitSemaphore != nullptr)
		{
			vkWaitSemaphore = static_cast<const VulkanSemaphore*>(waitSemaphore)->GetVkSemaphore();
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &vkWaitSemaphore;
			submitInfo.pWaitDstStageMask = waitStages;
		}
		else
		{
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
		}

		submitInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.Size());
		submitInfo.pCommandBuffers = vkCommandBuffers.Data();

		if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, vkFence) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to submit draw command buffer!");
			return false;
		}

		return SignalSubmissionIndex();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Shader* VulkanRhiDevice::CreateShader(Shader::ShaderType type)
	{
		return DefaultAllocator::GetInstance().New<VulkanShader>(type);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	GraphicsPipeline* VulkanRhiDevice::CreateGraphicsPipeline(const GraphicsPipeline::CreateInfo& createInfo)
	{
		VulkanGraphicsPipeline* mat = DefaultAllocator::GetInstance().New<VulkanGraphicsPipeline>();

		if (mat->Build(createInfo) == false)
		{
			DefaultAllocator::GetInstance().Delete(mat);
			return nullptr;
		}

		return mat;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	CommandBuffer* VulkanRhiDevice::CreateCommandBuffer()
	{
		return DefaultAllocator::GetInstance().New<VulkanCommandBuffer>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Buffer* VulkanRhiDevice::CreateBuffer(Buffer::Usage usage, uint32_t Size)
	{
		return DefaultAllocator::GetInstance().New<VulkanBuffer>(usage, Size);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Texture* VulkanRhiDevice::CreateTexture()
	{
		return DefaultAllocator::GetInstance().New<VulkanTexture>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	RenderTarget* VulkanRhiDevice::CreateRenderTarget()
	{
		return DefaultAllocator::GetInstance().New<VulkanRenderTarget>();
	}

	/// @brief
	/// @return
	Semaphore* VulkanRhiDevice::CreateSemaphore()
	{
		return DefaultAllocator::GetInstance().New<VulkanSemaphore>();
	}

	/// @brief
	/// @return
	Fence* VulkanRhiDevice::CreateFence()
	{
		return DefaultAllocator::GetInstance().New<VulkanFence>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	BindGroup* VulkanRhiDevice::CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
	                                            const BindGroup::TextureBinding* textureBindings, uint32_t textureBindingCount)
	{
		if (graphicsPipeline == nullptr)
		{
			return nullptr;
		}

		VulkanBindGroup* bindGroup = DefaultAllocator::GetInstance().New<VulkanBindGroup>();
		if (bindGroup->Build(*static_cast<const VulkanGraphicsPipeline*>(graphicsPipeline), set, uniformBuffers, uniformBufferCount, textureBindings, textureBindingCount) ==
		    false)
		{
			DefaultAllocator::GetInstance().Delete(bindGroup);
			return nullptr;
		}
		return bindGroup;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Sampler* VulkanRhiDevice::CreateSampler(const Sampler::CreateInfo& createInfo)
	{
		VulkanSampler* sampler = DefaultAllocator::GetInstance().New<VulkanSampler>(createInfo);
		if (sampler->Build() == false)
		{
			DefaultAllocator::GetInstance().Delete(sampler);
			return nullptr;
		}
		return sampler;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	uint32_t VulkanRhiDevice::GetUniformBufferOffsetAlignment() const
	{
		return (uint32_t)_selectedGpu->deviceProperties.limits.minUniformBufferOffsetAlignment;
	}
}
