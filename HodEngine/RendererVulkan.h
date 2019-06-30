#ifndef __RENDERER_VULKAN_HPP__
#define __RENDERER_VULKAN_HPP__

#include <vulkan.h>

#include "Renderer.h"

class RendererVulkan : public Renderer
{
public:
    RendererVulkan();
    virtual ~RendererVulkan();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) override;

    virtual bool GetPhysicalDeviceList(std::vector<GpuHelper::Device>* availableDevices) const override;

    virtual bool BuildPipeline(const GpuHelper::Device& physicalDevice) override;

    bool CreateDevice(const GpuHelper::Device& physicalDevice);
    bool CreateSwapChain();
    bool CreateCommandPool(const GpuHelper::Device& physicalDevice);
    bool CreateSemaphores();

    virtual bool DrawFrame() override;

    virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) override;
    virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader) override;

    VkInstance GetVkInstance() const;
    VkDevice GetVkDevice() const;
    VkRenderPass GetRenderPass() const;
    VkExtent2D GetSwapChainExtent() const;

private:
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
    VkPhysicalDevice physicalDevice;
    VkSwapchainKHR swapChain;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    VkExtent2D swapChainExtent;
};

#endif
