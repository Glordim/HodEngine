#include "RendererVulkan.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include "../RenderQueue.h"

#include "VkMesh.h"
#include "VkTexture.h"
#include "VkShader.h"
#include "VkMaterial.h"
#include "VkMaterialInstance.h"

RendererVulkan::RendererVulkan()
{
    this->currentImageIndex = 0;
    this->selectedGpu = nullptr;
    this->instance = VK_NULL_HANDLE;
    this->surface = VK_NULL_HANDLE;
    this->device = VK_NULL_HANDLE;
    this->graphicsQueue = VK_NULL_HANDLE;
    this->presentQueue = VK_NULL_HANDLE;
    this->swapChain = VK_NULL_HANDLE;
    this->renderPass = VK_NULL_HANDLE;
    this->commandPool = VK_NULL_HANDLE;
    this->descriptorPool = VK_NULL_HANDLE;
    this->imageAvailableSemaphore = VK_NULL_HANDLE;
    this->renderFinishedSemaphore = VK_NULL_HANDLE;
    this->acquireNextImageFence = VK_NULL_HANDLE;
}

RendererVulkan::~RendererVulkan()
{
    if (this->device != VK_NULL_HANDLE)
    {
        if (vkDeviceWaitIdle(this->device) != VK_SUCCESS)
            fprintf(stderr, "Vulkan: DeviceWaitIdel failed!\n");
    }

    if (this->acquireNextImageFence != VK_NULL_HANDLE)
        vkDestroyFence(this->device, this->acquireNextImageFence, nullptr);

    if (this->imageAvailableSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(this->device, this->imageAvailableSemaphore, nullptr);

    if (this->renderFinishedSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(this->device, this->renderFinishedSemaphore, nullptr);

    if (this->descriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(this->device, this->descriptorPool, nullptr);

    if (this->commandPool != VK_NULL_HANDLE)
        vkDestroyCommandPool(this->device, this->commandPool, nullptr);

    this->DestroySwapChain();

    if (this->device != VK_NULL_HANDLE)
        vkDestroyDevice(this->device, nullptr);

    if (this->surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

    if (this->instance != VK_NULL_HANDLE)
        vkDestroyInstance(this->instance, nullptr);
}

void RendererVulkan::DestroySwapChain()
{
    size_t swapChainFramebufferCount = this->swapChainFramebuffers.size();
    for (int i = 0; i < swapChainFramebufferCount; ++i)
    {
        vkDestroyFramebuffer(this->device, this->swapChainFramebuffers[i], nullptr);
    }
    this->swapChainFramebuffers.clear();

    size_t swapChainImageViewCount = this->swapChainImageViews.size();
    for (int i = 0; i < swapChainImageViewCount; ++i)
    {
        vkDestroyImageView(this->device, this->swapChainImageViews[i], nullptr);
    }
    this->swapChainImageViews.clear();

    if (this->renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(this->device, this->renderPass, nullptr);
        this->renderPass = VK_NULL_HANDLE;
    }

    if (this->swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);
        this->swapChain = VK_NULL_HANDLE;
    }
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

VkDescriptorPool RendererVulkan::GetDescriptorPool() const
{
    return this->descriptorPool;
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
        //"VK_LAYER_KHRONOS_validation"
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
    appInfo.apiVersion = VK_API_VERSION_1_1;

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

    return true;
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

bool RendererVulkan::GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices)
{
    if (availableDevices == nullptr)
        return false;

    if (this->availableGpu.empty() == true)
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);

        if (physicalDeviceCount == 0)
            return false;

        this->availableGpu.resize(physicalDeviceCount);

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data());

        for (size_t i = 0; i < physicalDeviceCount; ++i)
        {
            const VkPhysicalDevice& physicalDevice = physicalDevices[i];

            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
            vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

            VkGpuDevice& device = this->availableGpu[i];
            device.name = std::wstring(deviceProperties.deviceName, deviceProperties.deviceName + strlen(deviceProperties.deviceName));
            device.compatible = false;
            device.vram = 0;
            device.score = 0;

            device.score += deviceProperties.limits.maxImageDimension2D;

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                device.score += 1000;

            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &device.memProperties);

            size_t vram = 0;

            for (size_t j = 0; j < device.memProperties.memoryHeapCount; ++j)
            {
                const VkMemoryHeap& heap = device.memProperties.memoryHeaps[j];

                if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    if (vram < heap.size)
                        vram = heap.size;
                }
            }

            device.vram = vram;
            device.score += vram;

            device.physicalDevice = physicalDevice;
            device.graphicsAndPresentQueueFamilyIndex = 0;

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
                    if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, (uint32_t)j, this->surface, &presentSupport) != VK_SUCCESS)
                    {
                        fprintf(stderr, "Vulkan: Unable to check present support on FamilyQueue !\n");
                    }
                    else if (presentSupport == VK_TRUE)
                    {
                        device.graphicsAndPresentQueueFamilyIndex = (uint32_t)j;
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

                        for (size_t j = 0; j < formatCount; ++j)
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
    }
    
    size_t avalaibleDeviceCount = this->availableGpu.size();
    availableDevices->resize(avalaibleDeviceCount);

    for (size_t i = 0; i < avalaibleDeviceCount; ++i)
    {
        availableDevices->at(i) = (GpuDevice*)(&this->availableGpu[i]);
    }

    return true;
}

bool RendererVulkan::BuildPipeline(GpuDevice* physicalDevice)
{
    this->selectedGpu = (VkGpuDevice*)physicalDevice;

    if (this->CreateDevice() == false)
        return false;

    if (this->CreateCommandPool() == false)
        return false;

    if (this->CreateSwapChain() == false)
        return false;

    if (this->CreateSemaphores() == false)
        return false;

    Shader* vertexShader = this->CreateShader("Shader/UnlitVertexColor.vert.spirv", Shader::Vertex);
    if (vertexShader == nullptr)
        return false;

    Shader* fragmentShader = this->CreateShader("Shader/UnlitVertexColor.frag.spirv", Shader::Fragment);
    if (fragmentShader == nullptr)
        return false;

    // Extract descriptorSet definition from shader bytecode
    spirv_cross::Compiler compVert(((VkShader*)vertexShader)->GetShaderBytecode());
    spirv_cross::ShaderResources resourcesVert = compVert.get_shader_resources();

    size_t uniformBufferCount = resourcesVert.uniform_buffers.size();
    for (size_t i = 0; i < uniformBufferCount; ++i)
    {
        spirv_cross::Resource& resource = resourcesVert.uniform_buffers[i];

        size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

        if (set == 0)
        {
            this->viewLayout.ExtractBlockUbo(compVert, resource);
        }
        else if (set == 1)
        {
            this->modelLayout.ExtractBlockUbo(compVert, resource);
        }
    }

    spirv_cross::Compiler compFrag(((VkShader*)fragmentShader)->GetShaderBytecode());
    spirv_cross::ShaderResources resourcesFrag = compFrag.get_shader_resources();

    uniformBufferCount = resourcesFrag.uniform_buffers.size();
    for (size_t i = 0; i < uniformBufferCount; ++i)
    {
        spirv_cross::Resource& resource = resourcesFrag.uniform_buffers[i];

        size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

        if (set == 0)
        {
            this->viewLayout.ExtractBlockUbo(compFrag, resource);
        }
        else if (set == 1)
        {
            this->modelLayout.ExtractBlockUbo(compFrag, resource);
        }
    }

    this->viewLayout.BuildDescriptorSetLayout();
    this->modelLayout.BuildDescriptorSetLayout();

    this->unlitVertexColorMaterial = this->CreateMaterial(vertexShader, fragmentShader, Material::Topololy::TRIANGLE);
    this->unlitVertexColorMaterialInstance = this->CreateMaterialInstance(this->unlitVertexColorMaterial);

    this->unlitVertexColorLineMaterial = this->CreateMaterial(vertexShader, fragmentShader, Material::Topololy::LINE);
    this->unlitVertexColorLineMaterialInstance = this->CreateMaterialInstance(this->unlitVertexColorLineMaterial);

    return true;
}

bool RendererVulkan::CreateDevice()
{
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = this->selectedGpu->graphicsAndPresentQueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // Deprecated, now device share Validation Layer with the VkInstance
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(this->selectedGpu->physicalDevice, &createInfo, nullptr, &this->device) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create logical device !\n");
        return false;
    }

    vkGetDeviceQueue(this->device, this->selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->device, this->selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &this->presentQueue);

    return true;
}

VkDescriptorSetLayout RendererVulkan::GetVkViewDescriptorSet() const
{
    return this->viewLayout.GetDescriptorSetLayout();
}

VkDescriptorSetLayout RendererVulkan::GetVkModelDescriptorSet() const
{
    return this->modelLayout.GetDescriptorSetLayout();
}

bool RendererVulkan::CreateSwapChain()
{
    vkDeviceWaitIdle(this->device);

    this->DestroySwapChain();

    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->selectedGpu->physicalDevice, this->surface, &capabilities) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to get Surface capabilities !\n");
        return false;
    }

    if (capabilities.currentExtent.width == 0)
        return false;

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

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(this->device, &renderPassInfo, nullptr, &this->renderPass) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create render pass!\n");
        return false;
    }

    if (this->depthTexture.BuildDepth(extent.width, extent.height) == false)
        return false;

    // Image views
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);

    std::vector<VkImage> swapChainImages(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    this->swapChainImageViews.resize(imageCount, VK_NULL_HANDLE);
    for (size_t i = 0; i < imageCount; ++i)
    {
        // TODO use CreateImageView here ?

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

    // Framebuffers
    this->swapChainFramebuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        VkImageView attachments[] = {
            this->swapChainImageViews[i],
            this->depthTexture.GetTextureImageView()
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = this->renderPass;
        framebufferInfo.attachmentCount = 2;
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

bool RendererVulkan::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = this->selectedGpu->graphicsAndPresentQueueFamilyIndex;
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(this->device, &poolInfo, nullptr, &this->commandPool) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create command pool!\n");
        return false;
    }

    VkDescriptorPoolSize poolSizes[2];

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 150000; // TODO

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 150; // TODO

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = 2;
    descriptorPoolInfo.pPoolSizes = poolSizes;
    descriptorPoolInfo.maxSets = 150000; // TODO
    descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if (vkCreateDescriptorPool(this->device, &descriptorPoolInfo, nullptr, &this->descriptorPool) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create descriptor pool!\n");
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

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    fenceCreateInfo.pNext = nullptr;

    if (vkCreateFence(this->device, &fenceCreateInfo, nullptr, &this->acquireNextImageFence) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create fence!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = bufferUsage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(this->device, &bufferInfo, nullptr, buffer) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create buffer!\n");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

    uint32_t memoryTypeIndex = 0;
    if (this->FindMemoryTypeIndex(memRequirements.memoryTypeBits, memoryProperties, &memoryTypeIndex) == false)
    {
        fprintf(stderr, "Vulkan: Unable to find memory type for this buffer!\n");
        return false;
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to allocate buffer memory!\n");
        return false;
    }

    if (vkBindBufferMemory(this->device, *buffer, *bufferMemory, 0) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to bind buffer and buffer memory!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(this->device, &imageInfo, nullptr, image) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create image!\n");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(this->device, *image, &memRequirements);

    uint32_t memoryTypeIndex = 0;

    if (this->FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties, &memoryTypeIndex) == false)
        return false;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(this->device, &allocInfo, nullptr, imageMemory) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to allocate image memory!\n");
        return false;
    }

    if (vkBindImageMemory(this->device, *image, *imageMemory, 0) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to bind image and image memory!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView)
{
    *imageView = VK_NULL_HANDLE;

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(this->device, &viewInfo, nullptr, imageView) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Failed to create texture image view!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::CreateSampler(VkSampler* sampler)
{
    *sampler = VK_NULL_HANDLE;

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(this->device, &samplerInfo, nullptr, sampler) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Failed to create texture sampler!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::BeginSingleTimeCommands(VkCommandBuffer* commandBuffer)
{
    *commandBuffer = VK_NULL_HANDLE;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = this->commandPool;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(this->device, &allocInfo, commandBuffer) != VK_SUCCESS)
        return false;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        vkFreeCommandBuffers(this->device, this->commandPool, 1, commandBuffer);
        return false;
    }

    return true;
}

bool RendererVulkan::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    bool ret = false;
    VkSubmitInfo submitInfo = {};

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        goto exit;

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
        goto exit;

    if (vkQueueWaitIdle(graphicsQueue) != VK_SUCCESS)
        goto exit;

    ret = true;

exit:
    vkFreeCommandBuffers(this->device, this->commandPool, 1, &commandBuffer);

    return ret;
}

bool RendererVulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    
    if (this->BeginSingleTimeCommands(&commandBuffer) == false)
        return false;
    
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    if (this->EndSingleTimeCommands(commandBuffer) == false)
        return false;

    return true;
}

bool RendererVulkan::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        fprintf(stderr, "Vulkan: TransitionImageLayout, unsupported layout transition!\n");
        return false;
    }

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    if (this->BeginSingleTimeCommands(&commandBuffer) == false)
        return false;

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    if (this->EndSingleTimeCommands(commandBuffer) == false)
        return false;
}

bool RendererVulkan::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    
    if (this->BeginSingleTimeCommands(&commandBuffer) == false)
        return false;

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    if (this->EndSingleTimeCommands(commandBuffer) == false)
        return false;

    return true;
}

bool RendererVulkan::FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex)
{
    for (uint32_t i = 0; i < this->selectedGpu->memProperties.memoryTypeCount; ++i)
    {
        if ((memoryTypeBits & (1 << i)) && (this->selectedGpu->memProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
        {
            *memoryTypeIndex = i;
            return true;
        }
    }

    return false;
}

bool RendererVulkan::GenerateCommandBufferFromRenderQueue(RenderQueue& renderQueue, VkCommandBuffer* commandBuffer, std::vector<DescriptorSet*>& descriptorSetToCleanAfterRender)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffer) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create Command Buffer!\n");
        return false;
    }

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to begin recording command buffer!\n");
        return false;
    }

    VkClearValue clearColor[2];
    clearColor[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearColor[1] = { 1.0f, 0.0f, 0.0f, 0.0f };

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->renderPass;
    renderPassInfo.framebuffer = this->swapChainFramebuffers[this->currentImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = this->swapChainExtent;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearColor;

    vkCmdBeginRenderPass(*commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = (float)swapChainExtent.height;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = -(float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(*commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    vkCmdSetScissor(*commandBuffer, 0, 1, &scissor);

    const glm::mat4x4& projMatrix = renderQueue.GetProjMatrix();
    const glm::mat4x4& viewMatrix = renderQueue.GetViewMatrix();
    glm::mat4x4 vp = projMatrix * viewMatrix;

    DescriptorSet* viewDescriptorSet = new DescriptorSet();
    viewDescriptorSet->SetLayout(&this->viewLayout);
    viewDescriptorSet->SetUboValue("viewUbo.view", &viewMatrix, sizeof(glm::mat4x4));
    viewDescriptorSet->SetUboValue("viewUbo.proj", &projMatrix, sizeof(glm::mat4x4));
    viewDescriptorSet->SetUboValue("viewUbo.vp", &vp, sizeof(glm::mat4x4));

    glm::vec4 ambiantColor = glm::vec4(0.10f, 0.10f, 0.10f, 1.0f);
    glm::vec3 eyePos = renderQueue.GetCameraPos();

    viewDescriptorSet->SetUboValue("lightUbo.ambiantColor", &ambiantColor, sizeof(ambiantColor));
    viewDescriptorSet->SetUboValue("lightUbo.eyePos", &eyePos, sizeof(eyePos));

    const std::vector<RenderQueue::DirLightData*>& dirLightDatas = renderQueue.GetDirLightDatas();
    int32_t dirLightCount = dirLightDatas.size();

    viewDescriptorSet->SetUboValue("lightUbo.dirLightCount", &dirLightCount, sizeof(dirLightCount));

    for (size_t i = 0; i < dirLightCount; ++i)
    {
        RenderQueue::DirLightData* dirLightData = dirLightDatas[i];

        glm::vec4 color;
        color.x = dirLightData->dirLight->color.r;
        color.y = dirLightData->dirLight->color.g;
        color.z = dirLightData->dirLight->color.b;
        color.w = dirLightData->dirLight->color.a;

        viewDescriptorSet->SetUboValue("lightUbo.dirLight[" + std::to_string(i) + "].dir", &dirLightData->dir, sizeof(glm::vec3));
        viewDescriptorSet->SetUboValue("lightUbo.dirLight[" + std::to_string(i) + "].color", &color, sizeof(glm::vec4));
        viewDescriptorSet->SetUboValue("lightUbo.dirLight[" + std::to_string(i) + "].intensity", &dirLightData->dirLight->intensity, sizeof(float));
    }

    const std::vector<RenderQueue::PointLightData*>& pointLightDatas = renderQueue.GetPointLightDatas();
    int32_t pointLightCount = pointLightDatas.size();

    viewDescriptorSet->SetUboValue("lightUbo.pointLightCount", &pointLightCount, sizeof(pointLightCount));

    float constant = 1.0f;
    float linear = 0.14f;
    float quadratic = 0.07f;

    for (size_t i = 0; i < pointLightCount; ++i)
    {
        RenderQueue::PointLightData* pointLightData = pointLightDatas[i];

        glm::vec4 color;
        color.x = pointLightData->pointLight->color.r;
        color.y = pointLightData->pointLight->color.g;
        color.z = pointLightData->pointLight->color.b;
        color.w = pointLightData->pointLight->color.a;

        viewDescriptorSet->SetUboValue("lightUbo.pointLight[" + std::to_string(i) + "].pos", &pointLightData->pos, sizeof(glm::vec3));
        viewDescriptorSet->SetUboValue("lightUbo.pointLight[" + std::to_string(i) + "].color", &color, sizeof(glm::vec4));
        viewDescriptorSet->SetUboValue("lightUbo.pointLight[" + std::to_string(i) + "].intensity", &pointLightData->pointLight->intensity, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.pointLight[" + std::to_string(i) + "].constant", &constant, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.pointLight[" + std::to_string(i) + "].linear", &linear, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.pointLight[" + std::to_string(i) + "].quadratic", &quadratic, sizeof(float));
    }

    const std::vector<RenderQueue::SpotLightData*>& spotLightDatas = renderQueue.GetSpotLightDatas();
    int32_t spotLightCount = spotLightDatas.size();

    viewDescriptorSet->SetUboValue("lightUbo.spotLightCount", &spotLightCount, sizeof(spotLightCount));

    constant = 1.0f;
    linear = 0.045f;
    quadratic = 0.0075f;

    for (size_t i = 0; i < spotLightCount; ++i)
    {
        RenderQueue::SpotLightData* spotLightData = spotLightDatas[i];

        glm::vec4 color;
        color.x = spotLightData->spotLight->color.r;
        color.y = spotLightData->spotLight->color.g;
        color.z = spotLightData->spotLight->color.b;
        color.w = spotLightData->spotLight->color.a;

        float cosRadius = glm::cos(glm::radians(spotLightData->spotLight->radius));

        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].pos", &spotLightData->pos, sizeof(glm::vec3));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].dir", &spotLightData->dir, sizeof(glm::vec3));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].color", &color, sizeof(glm::vec4));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].intensity", &spotLightData->spotLight->intensity, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].constant", &constant, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].linear", &linear, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].quadratic", &quadratic, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].radius", &cosRadius, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].outer", &spotLightData->spotLight->outer, sizeof(float));
        viewDescriptorSet->SetUboValue("lightUbo.spotLight[" + std::to_string(i) + "].inner", &spotLightData->spotLight->inner, sizeof(float));
    }

    VkDescriptorSet vkViewDescriptorSet = viewDescriptorSet->GetDescriptorSet();

    descriptorSetToCleanAfterRender.push_back(viewDescriptorSet);

    VkMaterial* defaultMat = ((VkMaterialInstance*)this->unlitVertexColorMaterialInstance)->GetMaterial();

    vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, defaultMat->GetGraphicsPipeline());
    vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, defaultMat->GetPipelineLayout(), 0, 1, &vkViewDescriptorSet, 0, nullptr);

    const std::vector<RenderQueue::MeshData*>& meshDatas = renderQueue.GetMeshDatas();

    size_t meshCount = meshDatas.size();
    for (size_t i = 0; i < meshCount; ++i)
    {
        const RenderQueue::MeshData* meshData = meshDatas[i];

        VkMaterialInstance* materialInstance = (VkMaterialInstance*)meshData->materialInstance;
        if (materialInstance == nullptr)
        {
            materialInstance = (VkMaterialInstance*)this->unlitVertexColorMaterialInstance;
        }

        VkMaterial* material = materialInstance->GetMaterial();

        std::vector<VkDescriptorSet> descriptorSets = materialInstance->GetDescriptorSets();

        vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetGraphicsPipeline());
        if (descriptorSets.empty() == false)
            vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), 2, descriptorSets.size(), descriptorSets.data(), 0, nullptr);


        // Model descriptor set

        glm::mat4x4 mvp = vp * meshData->matrix;

        DescriptorSet* modelDescriptorSet = new DescriptorSet();
        modelDescriptorSet->SetLayout(&this->modelLayout);
        modelDescriptorSet->SetUboValue("modelUbo.mvp", &mvp, sizeof(mvp));
        modelDescriptorSet->SetUboValue("modelUbo.model", &meshData->matrix, sizeof(meshData->matrix));
        VkDescriptorSet vkModelDescriptorSet = modelDescriptorSet->GetDescriptorSet();

        descriptorSetToCleanAfterRender.push_back(modelDescriptorSet);

        vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), 1, 1, &vkModelDescriptorSet, 0, nullptr);

        VkMesh* mesh = (VkMesh*)meshData->mesh;
        VkBuffer vertexBuffer = mesh->GetVertexBuffer();
        VkBuffer indiceBuffer = mesh->GetIndiceBuffer();
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*commandBuffer, 0, 1, &vertexBuffer, offsets);
        vkCmdBindIndexBuffer(*commandBuffer, indiceBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(*commandBuffer, static_cast<uint32_t>(mesh->GetIndiceCount()), 1, 0, 0, 0);
    }
    
    const std::vector<RenderQueue::LineData*>& lineDatas = renderQueue.GetLineDatas();

    size_t lineCount = lineDatas.size();
    for (size_t i = 0; i < lineCount; ++i)
    {
        const RenderQueue::LineData* lineData = lineDatas[i];

        VkMaterialInstance* materialInstance = (VkMaterialInstance*)lineData->materialInstance;
        if (materialInstance == nullptr)
        {
            materialInstance = (VkMaterialInstance*)this->unlitVertexColorLineMaterialInstance;
        }

        VkMaterial* material = materialInstance->GetMaterial();

        std::vector<VkDescriptorSet> descriptorSets = materialInstance->GetDescriptorSets();

        vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetGraphicsPipeline());
        if (descriptorSets.empty() == false)
            vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), 2, descriptorSets.size(), descriptorSets.data(), 0, nullptr);


        // Model descriptor set

        glm::mat4x4 mvp = vp * lineData->matrix;

        DescriptorSet* modelDescriptorSet = new DescriptorSet();
        modelDescriptorSet->SetLayout(&this->modelLayout);
        modelDescriptorSet->SetUboValue("modelUbo.mvp", &mvp, sizeof(mvp));
        modelDescriptorSet->SetUboValue("modelUbo.model", &lineData->matrix, sizeof(lineData->matrix));
        VkDescriptorSet vkModelDescriptorSet = modelDescriptorSet->GetDescriptorSet();

        descriptorSetToCleanAfterRender.push_back(modelDescriptorSet);

        vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), 1, 1, &vkModelDescriptorSet, 0, nullptr);

        VkMesh* mesh = (VkMesh*)lineData->mesh;
        VkBuffer vertexBuffer = mesh->GetVertexBuffer();
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*commandBuffer, 0, 1, &vertexBuffer, offsets);
        vkCmdDraw(*commandBuffer, mesh->GetVertexCount(), 1, 0, 0);
    }

    const std::vector<RenderQueue::TriangleData*>& triangleDatas = renderQueue.GetTriangleDatas();

    size_t triangleCount = triangleDatas.size();
    for (size_t i = 0; i < triangleCount; ++i)
    {
        const RenderQueue::TriangleData* triangleData = triangleDatas[i];

        VkMaterialInstance* materialInstance = (VkMaterialInstance*)triangleData->materialInstance;
        if (materialInstance == nullptr)
        {
            materialInstance = (VkMaterialInstance*)this->unlitVertexColorMaterialInstance;
        }

        VkMaterial* material = materialInstance->GetMaterial();

        std::vector<VkDescriptorSet> descriptorSets = materialInstance->GetDescriptorSets();

        vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetGraphicsPipeline());
        if (descriptorSets.empty() == false)
            vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), 2, descriptorSets.size(), descriptorSets.data(), 0, nullptr);


        // Model descriptor set

        glm::mat4x4 mvp = vp * triangleData->matrix;

        DescriptorSet* modelDescriptorSet = new DescriptorSet();
        modelDescriptorSet->SetLayout(&this->modelLayout);
        modelDescriptorSet->SetUboValue("modelUbo.mvp", &mvp, sizeof(mvp));
        modelDescriptorSet->SetUboValue("modelUbo.model", &triangleData->matrix, sizeof(triangleData->matrix));
        VkDescriptorSet vkModelDescriptorSet = modelDescriptorSet->GetDescriptorSet();

        descriptorSetToCleanAfterRender.push_back(modelDescriptorSet);

        vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), 1, 1, &vkModelDescriptorSet, 0, nullptr);

        VkMesh* mesh = (VkMesh*)triangleData->mesh;
        VkBuffer vertexBuffer = mesh->GetVertexBuffer();
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*commandBuffer, 0, 1, &vertexBuffer, offsets);
        vkCmdDraw(*commandBuffer, mesh->GetVertexCount(), 1, 0, 0);
    }
    
    vkCmdEndRenderPass(*commandBuffer);

    if (vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to recording command buffer!\n");
        return false;
    }

    return true;
}

bool RendererVulkan::SubmitRenderQueue(RenderQueue& renderQueue)
{
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    
    std::vector<DescriptorSet*> descriptorSetToCleanAfterRender;
    descriptorSetToCleanAfterRender.reserve(renderQueue.GetMeshDatas().size() + renderQueue.GetLineDatas().size() + renderQueue.GetTriangleDatas().size() + 1);

    if (this->GenerateCommandBufferFromRenderQueue(renderQueue, &commandBuffer, descriptorSetToCleanAfterRender) == false)
        return false;

    VkFence fence = VK_NULL_HANDLE;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    fenceCreateInfo.pNext = nullptr;

    if (vkCreateFence(this->device, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to create Fence!\n");
        return false;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to submit draw command buffer!\n");
        vkDestroyFence(this->device, fence, nullptr);
        return false;
    }

    if (vkWaitForFences(this->device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to wait fence\n");
        vkDestroyFence(this->device, fence, nullptr);
        return false;
    }

    vkDestroyFence(this->device, fence, nullptr);

    vkFreeCommandBuffers(this->device, this->commandPool, 1, &commandBuffer);

    size_t descriptorSetToCleanAfterRenderCount = descriptorSetToCleanAfterRender.size();
    for (size_t i = 0; i < descriptorSetToCleanAfterRenderCount; ++i)
    {
        delete descriptorSetToCleanAfterRender[i];
    }

    return true;
}

bool RendererVulkan::ResizeSwapChain()
{
    return this->CreateSwapChain();
}

bool RendererVulkan::AcquireNextImageIndex()
{
    if (this->swapChain == nullptr) // For exemple if the window is hidden the size will be 0 and not swap chain are created
        return false;

    if (vkResetFences(this->device, 1, &this->acquireNextImageFence) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to reset fence!\n");
        return false;
    }

    VkResult result = vkAcquireNextImageKHR(this->device, this->swapChain, std::numeric_limits<uint64_t>::max(), VK_NULL_HANDLE, this->acquireNextImageFence, &this->currentImageIndex);
    if (result != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to acquire next image!\n");

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            fprintf(stderr, "Vulkan: VK_ERROR_OUT_OF_DATE_KHR recreating SwapChain...\n");
            this->ResizeSwapChain();
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            fprintf(stderr, "Vulkan: VK_SUBOPTIMAL_KHR recreating SwapChain...\n");
            this->ResizeSwapChain();
        }

        return false;
    }

    if (vkWaitForFences(this->device, 1, &this->acquireNextImageFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to wait fence\n");
        return false;
    }

    return true;
}

bool RendererVulkan::SwapBuffer()
{
    if (this->swapChain == nullptr) // For exemple if the window is hidden the size will be 0 and not swap chain are created
        return false;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pWaitSemaphores = nullptr;

    VkSwapchainKHR swapChains[] = { this->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &this->currentImageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(this->presentQueue, &presentInfo);
    if (result != VK_SUCCESS)
    {
        fprintf(stderr, "Vulkan: Unable to present frame!\n");

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            fprintf(stderr, "Vulkan: VK_ERROR_OUT_OF_DATE_KHR recreating SwapChain...\n");
            this->ResizeSwapChain();
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            fprintf(stderr, "Vulkan: VK_SUBOPTIMAL_KHR recreating SwapChain...\n");
            this->ResizeSwapChain();
        }

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

Material* RendererVulkan::CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy)
{
    VkMaterial* mat = new VkMaterial();

    if (mat->Build(vertexShader, fragmentShader, topololy) == false)
    {
        delete mat;
        return nullptr;
    }

    return mat;
}

MaterialInstance* RendererVulkan::CreateMaterialInstance(Material* material)
{
    VkMaterialInstance* materialInstance = new VkMaterialInstance();

    if (materialInstance->SetMaterial(material) == false)
    {
        delete materialInstance;
        return nullptr;
    }

    return materialInstance;
}

Mesh* RendererVulkan::CreateMesh(const std::string& path)
{
    VkMesh* mesh = new VkMesh();

    if (path.find(".fbx") != std::string::npos)
    {
        if (mesh->loadFbx(path.c_str()) == false)
        {
            delete mesh;
            return nullptr;
        }
    }
    else if (path.find(".obj") != std::string::npos)
    {
        if (mesh->loadObj(path.c_str()) == false)
        {
            delete mesh;
            return nullptr;
        }
    }
    else if (path == "")
    {
        // Do nothing
    }
    else
    {
        delete mesh;
        return nullptr;
    }

    return mesh;
}

Texture* RendererVulkan::CreateTexture(const std::string& path)
{
    VkTexture* texture = new VkTexture();

    if (path != "" && texture->LoadFromPath(path.c_str()) == false)
    {
        delete texture;
        return nullptr;
    }

    return texture;
}
