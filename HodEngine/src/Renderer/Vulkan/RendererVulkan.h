#pragma once

#include <vulkan/vulkan.h>

#include "../Renderer.h"

#include "VkGpuDevice.h"

#include "DescriptorSetLayout.h"

#include "VkTexture.h"

namespace HOD
{
    class DescriptorSet;

    class RendererVulkan : public Renderer
    {
    public:
        RendererVulkan() = default;
        ~RendererVulkan() override;

        virtual bool Init(SDL_Window* window, bool enableValidationLayers) override;
        virtual bool SetupImGui() override;

        virtual bool GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) override;

        virtual bool BuildPipeline(GpuDevice* gpuDevice) override;

        bool CreateDevice();
        bool CreateSwapChain();
        void DestroySwapChain();
        bool CreateCommandPool();
        bool CreateSemaphores();

        virtual bool SubmitRenderQueue(RenderQueue& renderQueue) override;

        virtual bool ResizeSwapChain() override;

        virtual bool AcquireNextImageIndex() override;
        virtual bool SwapBuffer() override;

        virtual Mesh* CreateMesh(const std::string& path) override;
        virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) override;
        virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) override;
        virtual MaterialInstance* CreateMaterialInstance(Material* material) override;
        virtual Texture* CreateTexture(const std::string& path) override;

        VkInstance GetVkInstance() const;
        VkDevice GetVkDevice() const;
        VkRenderPass GetRenderPass() const;
        VkExtent2D GetSwapChainExtent() const;
        VkDescriptorPool GetDescriptorPool() const;

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

    private:

        bool FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex);
        bool GenerateCommandBufferFromRenderQueue(RenderQueue& renderQueue, VkCommandBuffer* commandBuffer, std::vector<DescriptorSet*>& descriptorSetToCleanAfterRender);

        static void GetAvailableExtensions(std::vector<VkExtensionProperties>* availableExtensions);
        static bool GetExtensionRequiredBySDL(SDL_Window* window, std::vector<const char*>* extensionsRequiredBySDL);
        static bool CheckExtensionsIsAvailable(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions);
        static bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    private:
        VkInstance                  _instance = VK_NULL_HANDLE;
        VkSurfaceKHR                _surface = VK_NULL_HANDLE;
        VkDevice                    _device = VK_NULL_HANDLE;
        VkQueue                     _graphicsQueue = VK_NULL_HANDLE;
        VkQueue                     _presentQueue = VK_NULL_HANDLE;
        VkSwapchainKHR              _swapChain = VK_NULL_HANDLE;
        std::vector<VkImageView>    _swapChainImageViews;
        std::vector<VkFramebuffer>  _swapChainFramebuffers;
        VkTexture                   _depthTexture;
        VkRenderPass                _renderPass = VK_NULL_HANDLE;
        VkCommandPool               _commandPool = VK_NULL_HANDLE;
        VkDescriptorPool            _descriptorPool = VK_NULL_HANDLE;
        VkSemaphore                 _imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore                 _renderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence                     _acquireNextImageFence = VK_NULL_HANDLE;

        VkGpuDevice*                _selectedGpu = nullptr;
        std::vector<VkGpuDevice>    _availableGpu;

        VkExtent2D                  _swapChainExtent;

        uint32_t                    _currentImageIndex = 0;

        Material*                   _unlitVertexColorMaterial = nullptr;
        MaterialInstance*           _unlitVertexColorMaterialInstance = nullptr;

        Material*                   _unlitVertexColorLineMaterial = nullptr;
        MaterialInstance*           _unlitVertexColorLineMaterialInstance = nullptr;

        DescriptorSetLayout         _viewLayout;
        DescriptorSetLayout         _modelLayout;

        Mesh*                       _skyboxMesh = nullptr;
    };
}
