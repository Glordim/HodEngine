#pragma once

#include <vulkan/vulkan.h>

#include "HodEngine/Renderer/Renderer.h"

#include "HodEngine/Renderer/RHI/Vulkan/VkGpuDevice.h"

#include "HodEngine/Renderer/RHI/Vulkan/DescriptorSetLayout.h"

#include "HodEngine/Renderer/RHI/Vulkan/VkTexture.h"
#include "HodEngine/Renderer/RHI/Vulkan/VkContext.h"

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
	#undef max
#endif

namespace hod::renderer
{
	class DescriptorSet;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class RendererVulkan : public Renderer
	{
		_SingletonOverride(RendererVulkan)

	protected:

		~RendererVulkan() override;

	public:

		bool		Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) override;
		bool		BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier = 0) override;

		bool		CreateContext(window::Window* window); // TODO virtual in Renderer ?

		virtual bool GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) override;


		bool CreateDevice();
		bool CreateCommandPool();

		virtual bool ResizeSwapChain() override; // TODO remove

		virtual bool AcquireNextImageIndex() override { return false; }  // TODO remove
		virtual bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount) override;
		virtual bool SwapBuffer() override { return false; }  // TODO remove

		virtual CommandBuffer* CreateCommandBuffer() override;
		virtual Buffer* CreateBuffer(Buffer::Usage usage) override;
		virtual Shader* CreateShader(Shader::ShaderType type) override;
		virtual Material* CreateMaterial(VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) override;
		virtual MaterialInstance* CreateMaterialInstance(const Material* material) override;
		virtual Texture* CreateTexture() override;

		VkInstance GetVkInstance() const;
		VkDevice GetVkDevice() const;
		const VkGpuDevice* GetVkGpuDevice() const;
		VkDescriptorPool GetDescriptorPool() const;
		VkCommandPool GetCommandPool() const;
		VkQueue GetPresentQueue() const { return _presentQueue; }
		VkContext* GetContext() const { return _context; }

		Material* GetSharedMinimalMaterial() const;

		bool CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
		bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
		bool CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView);
		bool CreateSampler(VkSampler* sampler);

		bool BeginSingleTimeCommands(VkCommandBuffer* commandBuffer);
		bool EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		bool CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		bool TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);
		bool CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		VkDescriptorSetLayout GetVkViewDescriptorSet() const;
		VkDescriptorSetLayout GetVkModelDescriptorSet() const;

		bool FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex);

	private:

		bool			CreateVkIntance();
		VkSurfaceKHR	CreateSurface(window::Window* window);

		void	EnumeratePhysicalDevice(VkSurfaceKHR surface);
		void	FillPhysicalDeviceInfo(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkGpuDevice& gpuDevice);
		bool	SelectPhysicalDevice(uint32_t physicalDeviceIdentifier);

	private:

		static void GetAvailableExtensions(std::vector<VkExtensionProperties>& availableExtensions);
		static bool CheckExtensionsIsAvailable(const char** extensions, size_t extensionCount, const std::vector<VkExtensionProperties>& availableExtensions);

#if defined(renderer_ENABLE_VALIDATION_LAYER)
		static bool CheckValidationLayerSupport(const char** validationLayers, size_t validationLayerCount);
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif

	private:
		VkInstance                  _vkInstance = VK_NULL_HANDLE;
		VkDevice                    _device = VK_NULL_HANDLE;
		VkQueue                     _graphicsQueue = VK_NULL_HANDLE;
		VkQueue                     _presentQueue = VK_NULL_HANDLE;
		//VkSwapchainKHR              _swapChain = VK_NULL_HANDLE;
		//std::vector<VkImageView>    _swapChainImageViews;
		//std::vector<VkFramebuffer>  _swapChainFramebuffers;
		VkTexture                   _depthTexture;
		//VkRenderPass                _renderPass = VK_NULL_HANDLE;
		VkCommandPool               _commandPool = VK_NULL_HANDLE;
		VkDescriptorPool            _descriptorPool = VK_NULL_HANDLE;

		const VkGpuDevice* _selectedGpu = nullptr;
		const VkGpuDevice* _recommandedGpu = nullptr;
		std::vector<VkGpuDevice>	_availableGpu;

		Material* _unlitVertexColorMaterial = nullptr;
		MaterialInstance* _unlitVertexColorMaterialInstance = nullptr;

		Material* _unlitVertexColorLineMaterial = nullptr;
		MaterialInstance* _unlitVertexColorLineMaterialInstance = nullptr;

		DescriptorSetLayout			_viewLayout;
		DescriptorSetLayout			_modelLayout;
		Material* _sharedMinimalMaterial = nullptr;

		VkContext* _context = nullptr;
	};
}