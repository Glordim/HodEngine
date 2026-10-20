#pragma once
#include "HodEngine/RHI/Export.hpp"

#include <vulkan/vulkan.h>

#include "HodEngine/RHI/RhiDevice.hpp"

#include "HodEngine/RHI/Vulkan/VulkanGpuDevice.hpp"

#include "HodEngine/RHI/Enums.hpp"
#include "HodEngine/RHI/Vulkan/VulkanTexture.hpp"

#include <vk_mem_alloc.h>

#if defined(RHI_ENABLE_VALIDATION_LAYER)
	#include "HodEngine/RHI/Vulkan/VulkanValidationLayer.hpp"
#endif

#if defined(PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#undef max
	#undef CreateSemaphore
#endif

namespace hod::inline rhi
{

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API VulkanRhiDevice : public RhiDevice
	{
		_SingletonOverride(VulkanRhiDevice)

	protected:
		~VulkanRhiDevice() override;

	public:
		void WaitIdle() override;

		bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) override;

		PresentationSurface* CreatePresentationSurface(window::Window* window) override;

		bool CreateDevice();
		bool CreateCommandPool();

		bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr, const Semaphore* waitSemaphore = nullptr,
		                          const Fence* fence = nullptr) override;

		CommandBuffer*    CreateCommandBuffer() override;
		Buffer*           CreateBuffer(Buffer::Usage usage, uint32_t size) override;
		Shader*           CreateShader(Shader::ShaderType type) override;
		GraphicsPipeline* CreateGraphicsPipeline(const GraphicsPipeline::CreateInfo& createInfo) override;
		Texture*          CreateTexture() override;
		RenderTarget*     CreateRenderTarget() override;
		Semaphore*        CreateSemaphore() override;
		Fence*            CreateFence() override;
		BindGroup*        CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
		                                  const BindGroup::TextureBinding* textureBindings, uint32_t textureBindingCount) override;
		uint32_t          GetUniformBufferOffsetAlignment() const override;

		VkInstance         GetVkInstance() const;
		VkDevice           GetVkDevice() const;
		const VulkanGpuDevice* GetVkGpuDevice() const;
		// From one of the descriptor pools, another one being created when they are all full.
		// Released with DeferDestroy(descriptorSet, descriptorPool).
		bool AllocateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet* descriptorSet, VkDescriptorPool* descriptorPool);
		VkCommandPool      GetCommandPool() const;

		// One color attachment of colorFormat and one depth attachment of depthFormat, either of them PixelFormat::None, both cleared at its start.
		// The color is left in colorFinalLayout.
		bool CreateRenderPass(PixelFormat colorFormat, PixelFormat depthFormat, VkImageLayout colorFinalLayout, VkRenderPass* renderPass);
		// A render pass of these formats, owned by the device. The render passes of the same formats are all compatible with it:
		// a pipeline created with it can be drawn in any of them.
		VkRenderPass GetCompatibleRenderPass(PixelFormat colorFormat, PixelFormat depthFormat);

		VkQueue GetPresentQueue() const
		{
			return _presentQueue;
		}

		VmaAllocator GetVmaAllocator() const
		{
			return _vmaAllocator;
		}

		// GraphicsPipeline* GetSharedMinimalMaterial() const;

		bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image,
		                 VmaAllocation* imageMemory, uint32_t mipLevelCount = 1);
		bool CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView, uint32_t mipLevelCount = 1);

		// Commands that upload data into resources, recorded at any time without the CPU waiting for them: they are submitted ahead of
		// the next SubmitCommandBuffers, so that what they upload is ready for it. What is released meanwhile is kept until they are done.
		VkCommandBuffer GetUploadCommandBuffer();
		// A buffer, mapped, for an upload command to copy from. Released once the upload commands are done.
		bool CreateUploadBuffer(VkDeviceSize size, VkBuffer* buffer, void** mappedData);

		// The mip levels [0, mipLevelCount)
		bool TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout,
		                           uint32_t mipLevelCount = 1);
		void CopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage image, VkBuffer buffer, uint32_t width, uint32_t height);

		bool FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex);

		void DeferDestroy(VkFramebuffer framebuffer);
		void DeferDestroy(VkRenderPass renderPass);
		void DeferDestroy(VkSemaphore semaphore);
		void DeferDestroy(VkSampler sampler);
		void DeferDestroy(VkImageView imageView);
		void DeferDestroy(VkImage image, VmaAllocation allocation);
		void DeferDestroy(VkBuffer buffer, VmaAllocation allocation);
		void DeferDestroy(VkDescriptorSet descriptorSet, VkDescriptorPool descriptorPool);
		void DeferDestroy(VkPipeline pipeline);
		void DeferDestroy(VkPipelineLayout pipelineLayout);
		void DeferDestroy(VkCommandBuffer commandBuffer);

	protected:
		bool     InitDevice(uint32_t physicalDeviceIdentifier) override;
		Sampler* CreateSampler(const Sampler::CreateInfo& createInfo) override;
		void FlushDeferredDeletions(uint32_t frameIndex) override;

	private:
		bool CreateVkIntance();

		// Ends the pending upload commands, for the caller to submit them now. Null when there are none.
		VkCommandBuffer EndUploads();
		void            FlushUploads(); // submits them on their own, for WaitIdle

		// To call after each submission: gives it the next submission index
		bool SignalSubmissionIndex();
		void UpdateCompletedSubmissionIndex();
		// What is released now may be used by the submissions up to the next one, which carries the pending upload commands
		uint64_t GetDeferredDeletionSubmissionIndex() const;
		void     ReleaseCompletedDeletions();
		bool CreateDescriptorPool();

		void EnumeratePhysicalDevice();
		void FillPhysicalDeviceInfo(VkPhysicalDevice physicalDevice, VulkanGpuDevice& gpuDevice);
		bool SelectPhysicalDevice(uint32_t physicalDeviceIdentifier);

	private:
#if defined(RHI_ENABLE_VALIDATION_LAYER)
		VulkanValidationLayer _validationLayer;
#endif

		VkInstance       _vkInstance = VK_NULL_HANDLE;
		VkDevice         _device = VK_NULL_HANDLE;
		VkQueue          _graphicsQueue = VK_NULL_HANDLE;
		VkQueue          _presentQueue = VK_NULL_HANDLE;
		VkCommandPool    _commandPool = VK_NULL_HANDLE;
		Vector<VkDescriptorPool> _descriptorPools;

		const VulkanGpuDevice*  _selectedGpu = nullptr;
		const VulkanGpuDevice*  _recommandedGpu = nullptr;
		Vector<VulkanGpuDevice> _availableGpu;

		VmaAllocator _vmaAllocator = VK_NULL_HANDLE;

		struct CompatibleRenderPass
		{
			PixelFormat  _colorFormat = PixelFormat::None;
			PixelFormat  _depthFormat = PixelFormat::None;
			VkRenderPass _renderPass = VK_NULL_HANDLE;
		};
		Vector<CompatibleRenderPass> _compatibleRenderPasses;

		struct DeferredImage
		{
			VkImage       image      = VK_NULL_HANDLE;
			VmaAllocation allocation = VK_NULL_HANDLE;
		};

		struct DeferredBuffer
		{
			VkBuffer      buffer     = VK_NULL_HANDLE;
			VmaAllocation allocation = VK_NULL_HANDLE;
		};

		struct DeferredDescriptorSet
		{
			VkDescriptorSet  descriptorSet = VK_NULL_HANDLE;
			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		};

		// Released with the index of the next submission: destroyed once the GPU is done with it, the last one that may use them
		template<typename T>
		struct Deferred
		{
			T        _object = {};
			uint64_t _submissionIndex = 0;
		};

		Vector<Deferred<VkFramebuffer>>         _framebuffersToDestroy;
		Vector<Deferred<VkRenderPass>>          _renderPassesToDestroy;
		Vector<Deferred<VkSemaphore>>           _vkSemaphoresToDestroy;
		Vector<Deferred<VkSampler>>             _samplersToDestroy;
		Vector<Deferred<VkImageView>>           _imageViewsToDestroy;
		Vector<Deferred<DeferredImage>>         _imagesToDestroy;
		Vector<Deferred<DeferredBuffer>>        _buffersToDestroy;
		Vector<Deferred<DeferredDescriptorSet>> _descriptorSetsToDestroy;
		Vector<Deferred<VkPipeline>>            _pipelinesToDestroy;
		Vector<Deferred<VkPipelineLayout>>      _pipelineLayoutsToDestroy;
		Vector<Deferred<VkCommandBuffer>>       _commandBuffersToDestroy;

		// Every submission to the queue gets the next index, which a fence signals once the GPU is done with it, and so with the ones before
		struct SubmittedFence
		{
			VkFence  _fence = VK_NULL_HANDLE;
			uint64_t _submissionIndex = 0;
		};

		uint64_t               _lastSubmissionIndex = 0;
		uint64_t               _completedSubmissionIndex = 0; // the GPU is done with the submissions up to it
		Vector<SubmittedFence> _submittedFences;              // not signaled yet when last polled, in submission order
		Vector<VkFence>        _freeFences;

		VkCommandBuffer _uploadCommandBuffer = VK_NULL_HANDLE; // recording, until EndUploads
	};
}
