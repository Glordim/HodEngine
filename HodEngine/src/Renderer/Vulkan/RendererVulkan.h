#ifndef __RENDERER_VULKAN_HPP__
#define __RENDERER_VULKAN_HPP__

#include <vulkan.h>

#include "../Renderer.h"

#include "VkGpuDevice.h"

class RendererVulkan : public Renderer
{
public:
    RendererVulkan();
    virtual ~RendererVulkan();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) override;

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
    virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy = Material::Topololy::TRIANGLE) override;
    virtual MaterialInstance* CreateMaterialInstance(Material* material) override;
    virtual Texture* CreateTexture(const std::string& path) override;

    VkInstance GetVkInstance() const;
    VkDevice GetVkDevice() const;
    VkRenderPass GetRenderPass() const;
    VkExtent2D GetSwapChainExtent() const;
    VkDescriptorPool GetDescriptorPool() const;

    bool CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
    bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
    bool CreateImageView(VkImage image, VkFormat format, VkImageView* imageView);
    bool CreateSampler(VkSampler* sampler);

    bool BeginSingleTimeCommands(VkCommandBuffer* commandBuffer);
    bool EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    bool CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    bool TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    bool CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

private:

    bool FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex);
    bool GenerateCommandBufferFromRenderQueue(RenderQueue& renderQueue, VkCommandBuffer* commandBuffer);

    static void GetAvailableExtensions(std::vector<VkExtensionProperties>* availableExtensions);
    static bool GetExtensionRequiredBySDL(SDL_Window* window, std::vector<const char*>* extensionsRequiredBySDL);
    static bool CheckExtensionsIsAvailable(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions);
    static bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

private:
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    VkDescriptorPool descriptorPool;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence acquireNextImageFence;

    VkGpuDevice* selectedGpu;
    std::vector<VkGpuDevice> availableGpu;

    VkExtent2D swapChainExtent;

    uint32_t currentImageIndex;

    Material* unlitVertexColorMaterial;
    MaterialInstance* unlitVertexColorMaterialInstance;

    Material* unlitVertexColorLineMaterial;
    MaterialInstance* unlitVertexColorLineMaterialInstance;
};

#endif
