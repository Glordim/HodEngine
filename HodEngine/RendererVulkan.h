#ifndef __RENDERER_VULKAN_HPP__
#define __RENDERER_VULKAN_HPP__

#include <vulkan.h>

#include "Renderer.h"

#include <vector>

class RendererVulkan : public Renderer
{
public:
    RendererVulkan();
    virtual ~RendererVulkan();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) override;
    virtual bool CreateDevice() override;

private:
    static void GetAvailableExtensions(std::vector<VkExtensionProperties>* availableExtensions);
    static bool GetExtensionRequiredBySDL(SDL_Window* window, std::vector<const char*>* extensionsRequiredBySDL);
    static bool CheckExtensionsIsAvailable(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions);
    static bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

private:
    VkInstance instance;
};

#endif