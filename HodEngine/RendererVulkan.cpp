#include "RendererVulkan.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include "VkShader.h"
#include "VkMaterial.h"

RendererVulkan::RendererVulkan()
{
    this->instance = VK_NULL_HANDLE;
    this->surface = VK_NULL_HANDLE;
    this->device = VK_NULL_HANDLE;
    this->graphicsQueue = VK_NULL_HANDLE;
    this->presentQueue = VK_NULL_HANDLE;
    this->physicalDevice = VK_NULL_HANDLE;
    this->swapChain = VK_NULL_HANDLE;
    this->renderPass = VK_NULL_HANDLE;
    this->commandPool = VK_NULL_HANDLE;
    this->imageAvailableSemaphore = VK_NULL_HANDLE;
    this->renderFinishedSemaphore = VK_NULL_HANDLE;
}

RendererVulkan::~RendererVulkan()
{
    if (this->device != VK_NULL_HANDLE)
    {
        if (vkDeviceWaitIdle(this->device) != VK_SUCCESS)
            fprintf(stderr, "Vulkan: DeviceWaitIdel failed!\n");
    }

    if (this->imageAvailableSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(this->device, this->imageAvailableSemaphore, nullptr);

    if (this->renderFinishedSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(this->device, this->renderFinishedSemaphore, nullptr);

    if (this->commandPool != VK_NULL_HANDLE)
        vkDestroyCommandPool(this->device, this->commandPool, nullptr);

    size_t swapChainFramebufferCount = this->swapChainFramebuffers.size();
    for (int i = 0; i < swapChainFramebufferCount; ++i)
    {
        vkDestroyFramebuffer(this->device, this->swapChainFramebuffers[i], nullptr);
    }

    size_t swapChainImageViewCount = this->swapChainImageViews.size();
    for (int i = 0; i < swapChainImageViewCount; ++i)
    {
        vkDestroyImageView(this->device, this->swapChainImageViews[i], nullptr);
    }

    if (this->renderPass != VK_NULL_HANDLE)
        vkDestroyRenderPass(this->device, this->renderPass, nullptr);

    if (this->swapChain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);

    if (this->device != VK_NULL_HANDLE)
        vkDestroyDevice(this->device, nullptr);

    if (this->surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

    if (this->instance != VK_NULL_HANDLE)
        vkDestroyInstance(this->instance, nullptr);
}

VkInstance RendererVulkan::GetVkInstance() const
{
    return this->instance;
}

VkDevice RendererVulkan::GetVkDevice() const
{
    return this->device;
}

VkRenderPass RendererVulkan::GetRenderPass() const
{
    return this->renderPass;
}

VkExtent2D RendererVulkan::GetSwapChainExtent() const
{
    return this->swapChainExtent;
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
        //"VK_LAYER_LUNARG_standard_validation"
        "VK_LAYER_KHRONOS_validation"
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

    if (SDL_Vulkan_CreateSurface(window, this->instance, &this->surface) == SDL_FALSE)
    {
        fprintf(stderr, "SDL: Unable to create Vulkan surface for SDL Window: %s\n", SDL_GetError());
        return false;
    }

    return true;
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

bool RendererVulkan::GetPhysicalDeviceList(std::vector<GpuHelper::Device>* availableDevices) const
{
    if (availableDevices == nullptr)
        return false;

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
        return false;

    availableDevices->resize(physicalDeviceCount);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data());

    for (size_t i = 0; i < physicalDeviceCount; ++i)
    {
        const VkPhysicalDevice& physicalDevice = physicalDevices[i];

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        GpuHelper::Device& device = availableDevices->at(i);
        device.name = deviceProperties.deviceName;
        device.compatible = false;
        device.score = 0;
        
        device.score += deviceProperties.limits.maxImageDimension2D;

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            device.score += 1000;

        device.handle = (void*)physicalDevice;
        device.handleQueueIndex = 0;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        for (size_t j = 0; j < queueFamilyCount; ++j)
        {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[j];

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                VkBool32 presentSupport = VK_FALSE;
                if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, j, this->surface, &presentSupport) != VK_SUCCESS)
                {
                    fprintf(stderr, "Vulkan: Unable to check present support on FamilyQueue !\n");
                }
                else if (presentSupport == VK_TRUE)
                {
                    device.handleQueueIndex = j;
                    device.compatible = true;
                    break;
                }
            }
        }

        if (deviceFeatures.geometryShader == VK_FALSE)
            device.compatible = false;

        const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        uint32_t availableExtensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, availableExtensions.data());

        if (RendererVulkan::CheckExtensionsIsAvailable(requiredExtensions, availableExtensions) == false)
            device.compatible = false;

        if (device.compatible == true)
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;

            if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, this->surface, &capabilities) != VK_SUCCESS)
            {
                fprintf(stderr, "Vulkan: Unable to get Surface capabilities !\n");
                device.compatible = false;
            }
            else
            {
                uint32_t formatCount;
                if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, this->surface, &formatCount, nullptr) != VK_SUCCESS)
                {
                    fprintf(stderr, "Vulkan: Unable to get Surface formats !\n");
                    device.compatible = false;
                }

                if (formatCount != 0)
                {
                    formats.resize(formatCount);
                    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, this->surface, &formatCount, formats.data()) != VK_SUCCESS)
                    {
                        fprintf(stderr, "Vulkan: Unable to get Surface formats !\n");
                        device.compatible = false;
                    }

                    bool targetFormatFounded = false;

                    for (int j = 0; j < formatCount; ++j)
                    {
                        const VkSurfaceFormatKHR& format = formats[j];

                        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                        {
                            targetFormatFounded = true;
                            break;
                        }
                    }

                    if (targetFormatFounded == false)
                        device.compatible = false;
                }
                else
                {
                    fprintf(stderr, "Vulkan: No Surface formats !\n");
                    device.compatible = false;
                }

                uint32_t presentModeCount;
                if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, this->surface, &presentModeCount, nullptr) != VK_SUCCESS)
                {
                    fprintf(stderr, "Vulkan: Unable to get Surface present modes !\n");
                    device.compatible = false;
                }

                if (presentModeCount != 0)
                {
                    presentModes.resize(presentModeCount);
                    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, this->surface, &presentModeCount, presentModes.data()) != VK_SUCCESS)
                    {
                        fprintf(stderr, "Vulkan: Unable to get Surface present modes !\n");
                        device.compatible = false;
                    }
                }
                else
                {
                    fprintf(stderr, "Vulkan: No Surface present modes !\n");
                    device.compatible = false;
                }
            }
        }
    }
    
    return true;
}

bool RendererVulkan::BuildPipeline(const GpuHelper::Device& physicalDevice)
{
    if (this->CreateDevice(physicalDevice) == false)
        return false;

    if (this->CreateSwapChain() == false)
        return false;

    if (this->CreateCommandPool(physicalDevice) == false)
        return false;

    if (this->CreateSemaphores() == false)
        return false;

    return true;
}

bool RendererVulkan::CreateDevice(const GpuHelper::Device& gpu)
{
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = gpu.handleQueueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // Deprecated, now device share Validation Layer with the VkInstance
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledLayerCount = 0;

    this->physicalDevice = (VkPhysicalDevice)gpu.handle;

    if (vkCreateDevice(this->physicalDevice, &createInfo, nullptr, &this->device) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create logical device !\n");
        return false;
    }

    vkGetDeviceQueue(this->device, gpu.handleQueueIndex, 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->device, gpu.handleQueueIndex, 0, &this->presentQueue);

    return true;
}

bool RendererVulkan::CreateSwapChain()
{
    // Render pass
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(this->device, &renderPassInfo, nullptr, &this->renderPass) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create render pass!\n");
        return false;
    }

    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &capabilities) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to get Surface capabilities !\n");
        return false;
    }

    VkExtent2D extent;

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        extent = capabilities.currentExtent;
    }
    else
    {
        extent.width = 800;
        extent.height = 600;

        if (extent.width > capabilities.maxImageExtent.width)
            extent.width = capabilities.maxImageExtent.width;
        else if (extent.width < capabilities.minImageExtent.width)
            extent.width = capabilities.minImageExtent.width;

        if (extent.height > capabilities.maxImageExtent.height)
            extent.height = capabilities.maxImageExtent.height;
        else if (extent.height < capabilities.minImageExtent.height)
            extent.height = capabilities.minImageExtent.height;
    }

    this->swapChainExtent = extent;

    uint32_t imageCount = capabilities.minImageCount + 1;

    if (imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = this->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent = this->swapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Present and graphics queue have the same queue family
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR; // Todo support VSync
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(this->device, &createInfo, nullptr, &this->swapChain) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create SwapChain !\n");
        return false;
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);

    std::vector<VkImage> swapChainImages(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    this->swapChainImageViews.resize(imageCount, VK_NULL_HANDLE);
    for (size_t i = 0; i < imageCount; ++i)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(this->device, &createInfo, nullptr, &this->swapChainImageViews[i]) != VK_SUCCESS)
        {
            fprintf(stderr, "Vulkan: Unable to create Image Views !\n");
            return false;
        }
    }

    this->swapChainFramebuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        VkImageView attachments[] = {
            this->swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(this->device, &framebufferInfo, nullptr, &this->swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            fprintf(stderr, "Vulkan: Unable to create Framebuffer !\n");
            return false;
        }
    }

    return true;
}

bool RendererVulkan::CreateCommandPool(const GpuHelper::Device& physicalDevice)
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = physicalDevice.handleQueueIndex;
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(this->device, &poolInfo, nullptr, &this->commandPool) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create command pool!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::CreateSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(this->device, &semaphoreInfo, nullptr, &this->imageAvailableSemaphore) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create semaphores!\n");
        return false;
    }

    if (vkCreateSemaphore(this->device, &semaphoreInfo, nullptr, &this->renderFinishedSemaphore) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create semaphores!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::DrawFrame()
{
    uint32_t imageIndex = 0;
    if (vkAcquireNextImageKHR(this->device, this->swapChain, std::numeric_limits<uint64_t>::max(), this->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to acquire next image!\n");
        return false;
    }

    VkSemaphore signalSemaphores[] = { this->renderFinishedSemaphore };
    VkSemaphore waitSemaphores[] = { this->imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    //submitInfo.commandBufferCount = 1;
    //submitInfo.pCommandBuffers = &this->commandBuffers[imageIndex];
    submitInfo.commandBufferCount = 0;
    submitInfo.pCommandBuffers = nullptr;

    if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to submit draw command buffer!\n");
        return false;
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { this->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    if (vkQueuePresentKHR(this->presentQueue, &presentInfo) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to present frame!\n");
        return false;
    }

    return true;
}

Shader* RendererVulkan::CreateShader(const std::string& path, Shader::ShaderType type)
{
    VkShader* shader = new VkShader(type);

    if (shader->LoadFromFile(path) == false)
    {
        delete shader;
        return nullptr;
    }

    return shader;
}

Material* RendererVulkan::CreateMaterial(Shader* vertexShader, Shader* fragmentShader)
{
    VkMaterial* mat = new VkMaterial();

    if (mat->Build(vertexShader, fragmentShader) == false)
    {
        delete mat;
        return nullptr;
    }

    return mat;
}
