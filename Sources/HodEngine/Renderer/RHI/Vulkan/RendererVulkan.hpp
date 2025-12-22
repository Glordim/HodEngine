#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vulkan/vulkan.h>

#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/VkGpuDevice.hpp"

#include "HodEngine/Renderer/Enums.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkTexture.hpp"

#include <VulkanMemoryAllocator/vk_mem_alloc.h>

#if defined(PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#undef max
	#undef CreateSemaphore
#endif

namespace hod::renderer
{
	class DescriptorSet;

	struct SamplerCreateInfo
	{
		FilterMode _filterMode = FilterMode::Linear;
		WrapMode   _wrapMode = WrapMode::Clamp;
	};

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API RendererVulkan : public Renderer
	{
		_SingletonOverride(RendererVulkan)

	protected:
		~RendererVulkan() override;

	public:
		bool Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) override;

		// bool CreateContext(window::Window* window); // TODO virtual in Renderer ?

		bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) override;

		bool CreateDevice();
		bool CreateCommandPool();

		bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr, const Semaphore* waitSemaphore = nullptr,
		                          const Fence* fence = nullptr) override;

		CommandBuffer*    CreateCommandBuffer() override;
		Buffer*           CreateBuffer(Buffer::Usage usage, uint32_t size) override;
		Shader*           CreateShader(Shader::ShaderType type) override;
		Material*         CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
		                                 Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE,
		                                 bool useDepth = true) override;
		MaterialInstance* CreateMaterialInstance(const Material* material) override;
		Texture*          CreateTexture() override;
		RenderTarget*     CreateRenderTarget() override;
		Semaphore*        CreateSemaphore() override;
		Fence*            CreateFence() override;

		VkInstance         GetVkInstance() const;
		VkDevice           GetVkDevice() const;
		const VkGpuDevice* GetVkGpuDevice() const;
		VkDescriptorPool   GetDescriptorPool() const;
		VkCommandPool      GetCommandPool() const;
		VkRenderPass       GetDummyRenderPass() const;

		VkQueue GetPresentQueue() const
		{
			return _presentQueue;
		}

		VmaAllocator GetVmaAllocator() const
		{
			return _vmaAllocator;
		}

		// Material* GetSharedMinimalMaterial() const;

		bool CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
		bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image,
		                 VmaAllocation* imageMemory);
		bool CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView);
		bool CreateSampler(VkSampler* sampler, const SamplerCreateInfo& createInfo);

		bool BeginSingleTimeCommands(VkCommandBuffer* commandBuffer);
		bool EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		bool CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		bool TransitionImageLayoutImmediate(VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);
		bool TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);
		bool CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		bool CopyImageToBuffer(VkImage image, VkBuffer buffer, uint32_t width, uint32_t height);

		bool FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex);

		static bool GetPhysicalDeviceSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR& capabilities);
		static bool GetPhysicalDeviceSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, Vector<VkSurfaceFormatKHR>& formats);
		static bool GetPhysicalDeviceSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, Vector<VkPresentModeKHR>& presentModes);

	private:
		bool CreateVkIntance();

		VkSurfaceKHR CreateSurface(window::Window* window);

		void EnumeratePhysicalDevice(VkSurfaceKHR surface);
		void FillPhysicalDeviceInfo(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkGpuDevice& gpuDevice);
		bool SelectPhysicalDevice(uint32_t physicalDeviceIdentifier);

	private:
		static void GetAvailableExtensions(Vector<VkExtensionProperties>& availableExtensions);
		static bool CheckExtensionsIsAvailable(const char** extensions, size_t extensionCount, const Vector<VkExtensionProperties>& availableExtensions);

#if defined(RENDERER_ENABLE_VALIDATION_LAYER)
		static bool                           CheckValidationLayerSupport(const char** validationLayers, size_t validationLayerCount);
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif

	private:
		VkInstance       _vkInstance = VK_NULL_HANDLE;
		VkDevice         _device = VK_NULL_HANDLE;
		VkQueue          _graphicsQueue = VK_NULL_HANDLE;
		VkQueue          _presentQueue = VK_NULL_HANDLE;
		VkCommandPool    _commandPool = VK_NULL_HANDLE;
		VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
		VkRenderPass     _dummyRenderPass = VK_NULL_HANDLE;

		const VkGpuDevice*  _selectedGpu = nullptr;
		const VkGpuDevice*  _recommandedGpu = nullptr;
		Vector<VkGpuDevice> _availableGpu;

		VmaAllocator _vmaAllocator = VK_NULL_HANDLE;
	};
}
