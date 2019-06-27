#include "RendererVulkan.h"

#include <SDL.h>
#include <SDL_vulkan.h>

struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
    bool founded;

    bool isComplete()
    {
        return founded;
    }
};

RendererVulkan::RendererVulkan()
{
    this->instance = VK_NULL_HANDLE;
}

RendererVulkan::~RendererVulkan()
{
    vkDestroyInstance(this->instance, nullptr);
}

bool RendererVulkan::Init(SDL_Window* window, bool enableValidationLayers)
{
    // === Extensions ===

    std::vector<const char*> extensionsRequiredByEngine = {
        // Nothing for the moment :P
    };

    std::vector<const char*> extensionsRequiredByValidationLayers = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

    std::vector<const char*> extensionsRequiredBySDL;

    if (RendererVulkan::GetExtensionRequiredBySDL(window, &extensionsRequiredBySDL) == false)
        return false;

    std::vector<VkExtensionProperties> availableExtensions;

    RendererVulkan::GetAvailableExtensions(&availableExtensions);

    if (RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredByEngine, availableExtensions) == false)
    {
        fprintf(stderr, "Vulkan: Extensions required by the Engine are not available, try to update 'Vulkan Runtime'\n");
        return false;
    }

    if (RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredBySDL, availableExtensions) == false)
    {
        fprintf(stderr, "Vulkan: Extensions required by SDL are not available, try to update 'Vulkan Runtime'\n");
        return false;
    }

    if (enableValidationLayers == true && RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredByValidationLayers, availableExtensions) == false)
    {
        fprintf(stderr, "Vulkan: Extensions required by ValidationLayers are not available, try to update 'Vulkan Runtime'\n");
        fprintf(stderr, "Vulkan: ValidationLayers have been disabled\n");
        enableValidationLayers = false;
    }

    // === Validation Layers ===

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
    };

    if (enableValidationLayers == true && RendererVulkan::CheckValidationLayerSupport(validationLayers) == false)
    {
        fprintf(stderr, "Vulkan: ValidationLayers are not available, try to update 'Vulkan Runtime'\n");
        fprintf(stderr, "Vulkan: ValidationLayers have been disabled\n");
        enableValidationLayers = false;
    }

    // === VkInstance CreateInfo ===

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (enableValidationLayers == true)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    std::vector<const char*> extensions;

    if (enableValidationLayers == true)
    {
        extensions.reserve(extensionsRequiredByEngine.size() + extensionsRequiredBySDL.size() + extensionsRequiredByValidationLayers.size());
        extensions.insert(extensions.end(), extensionsRequiredByEngine.begin(), extensionsRequiredByEngine.end());
        extensions.insert(extensions.end(), extensionsRequiredBySDL.begin(), extensionsRequiredBySDL.end());
        extensions.insert(extensions.end(), extensionsRequiredByValidationLayers.begin(), extensionsRequiredByValidationLayers.end());
    }
    else
    {
        extensions.reserve(extensionsRequiredByEngine.size() + extensionsRequiredBySDL.size());
        extensions.insert(extensions.end(), extensionsRequiredByEngine.begin(), extensionsRequiredByEngine.end());
        extensions.insert(extensions.end(), extensionsRequiredBySDL.begin(), extensionsRequiredBySDL.end());
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();


    // === Debug Messenger ===

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};

    if (enableValidationLayers == true)
    {
        debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerCreateInfo.pfnUserCallback = &RendererVulkan::DebugCallback;
        debugMessengerCreateInfo.pUserData = nullptr;

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
    }
    else
    {
        createInfo.pNext = nullptr;
    }

    // === Create Instance ===

    if (vkCreateInstance(&createInfo, nullptr, &this->instance) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create a Vulkan instance !\n");
        return false;
    }
}

void RendererVulkan::GetAvailableExtensions(std::vector<VkExtensionProperties>* availableExtensions)
{
    uint32_t availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

    availableExtensions->resize(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions->data());
}

bool RendererVulkan::GetExtensionRequiredBySDL(SDL_Window* window, std::vector<const char*>* extensionsRequiredBySDL)
{
    unsigned int extensionsRequiredBySDLCount = 0;
    if (SDL_Vulkan_GetInstanceExtensions(window, &extensionsRequiredBySDLCount, nullptr) == SDL_FALSE)
    {
        fprintf(stderr, "Vulkan: Unable to get Extensions required by SDL: %s\n", SDL_GetError());
        return false;
    }

    extensionsRequiredBySDL->resize(extensionsRequiredBySDLCount);
    if (SDL_Vulkan_GetInstanceExtensions(window, &extensionsRequiredBySDLCount, extensionsRequiredBySDL->data()) == SDL_FALSE)
    {
        fprintf(stderr, "Vulkan: Unable to get Extensions required by SDL: %s\n", SDL_GetError());
        return false;
    }
}

bool RendererVulkan::CheckExtensionsIsAvailable(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions)
{
    size_t availableExtensionsCount = availableExtensions.size();
    size_t extensionsCount = extensions.size();

    for (size_t i = 0; i < extensionsCount; ++i)
    {
        const char* extension = extensions[i];

        bool founded = false;

        for (size_t j = 0; j < availableExtensionsCount; ++j)
        {
            if (strcmp(extension, availableExtensions[j].extensionName) == 0)
            {
                founded = true;
                break;
            }
        }

        if (founded == false)
            return false;
    }

    return true;
}

bool RendererVulkan::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    uint32_t availableValidationLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, nullptr);

    std::vector<VkLayerProperties> availableValidationLayers(availableValidationLayerCount);
    vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, availableValidationLayers.data());

    size_t validationLayerCount = validationLayers.size();
    for (size_t i = 0; i < validationLayerCount; ++i)
    {
        const char* validationLayerName = validationLayers[i];

        bool founded = false;

        for (size_t j = 0; j < availableValidationLayerCount; ++j)
        {
            if (strcmp(validationLayerName, availableValidationLayers[j].layerName) == 0)
            {
                founded = true;
                break;
            }
        }

        if (founded == false)
            return false;
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL RendererVulkan::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{

    fprintf(stderr, "Validation Layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

std::vector<Renderer::PhysicalDevice> RendererVulkan::GetPhysicalDeviceList() const
{
    std::vector<Renderer::PhysicalDevice> devices;

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
    {
        return devices;
    }

    devices.resize(physicalDeviceCount);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data());

    for (int i = 0; i < physicalDeviceCount; ++i)
    {
        const VkPhysicalDevice& physicalDevice = physicalDevices[i];

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        Renderer::PhysicalDevice& device = devices[i];
        device.name = deviceProperties.deviceName;
        device.compatible = (deviceFeatures.geometryShader == VK_TRUE);

        device.score = deviceProperties.limits.maxImageDimension2D;

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            device.score += 1000;
    }
    
    return devices;
}

bool RendererVulkan::CreateDevice(const Renderer::PhysicalDevice& physicalDevice)
{
    /*
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        fprintf(stderr, "Vulkan: Unable to find GPUs with Vulkan support !\n");
        return 1;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(this->instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
        SDL_Log("Unable to find a suitable GPU !");
        return 1;
    }

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    QueueFamilyIndices indices;
    indices.graphicsFamily = 0;
    indices.founded = false;

    for (int i = 0; i < queueFamilyCount; ++i)
    {
        const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }
    }

    VkDevice device = VK_NULL_HANDLE;

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers == true)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    {
        SDL_Log("Vulkan : Unable to create logical device!");
        return 1;
    }

    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    */

    return false;
}
