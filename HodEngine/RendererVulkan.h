#ifndef __RENDERER_VULKAN_HPP__
#define __RENDERER_VULKAN_HPP__

#include <vulkan.h>

#include "Renderer.h"

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
    bool CreateCommandPool();
    bool CreateSemaphores();

    virtual bool SubmitRenderQueue(RenderQueue& renderQueue) override;

    virtual bool SwapBuffer() override;

    virtual Mesh* CreateMesh(const std::string& path) override;
    virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) override;
    virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader) override;

    VkInstance GetVkInstance() const;
    VkDevice GetVkDevice() const;
    VkRenderPass GetRenderPass() const;
    VkExtent2D GetSwapChainExtent() const;

    bool CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

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
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    VkGpuDevice* selectedGpu;
    std::vector<VkGpuDevice> availableGpu;

    VkExtent2D swapChainExtent;
};

#endif
