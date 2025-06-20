#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include "HodEngine/Renderer/RenderQueue.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/CommandBufferVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkTexture.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkRenderTarget.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterial.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderGeneratorGLSL.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/SemaphoreVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/FenceVk.hpp"
#include "HodEngine/Renderer/MaterialManager.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Window/Window.hpp>

#undef min
#undef max
#include <spirv_cross/spirv_cross.hpp>

#include <array>

#if defined(PLATFORM_WINDOWS)
	#include <vulkan/vulkan_win32.h>
	#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp>
#elif defined(PLATFORM_LINUX)
	#include <vulkan/vulkan_wayland.h>
	#include <HodEngine/Window/Desktop/Linux/Wayland/WaylandDisplayManager.hpp>
	#include <HodEngine/Window/Desktop/Linux/Wayland/WaylandWindow.hpp>
#elif defined(PLATFORM_ANDROID)
	#include <vulkan/vulkan_android.h>
	#include <HodEngine/Window/Android/AndroidWindow.hpp>
#endif

namespace hod::renderer
{
	_SingletonOverrideConstructor(RendererVulkan)
	: Renderer()
	{

	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	RendererVulkan::~RendererVulkan()
	{
		vmaDestroyAllocator(_vmaAllocator);

		if (_device != VK_NULL_HANDLE)
		{
			if (vkDeviceWaitIdle(_device) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: DeviceWaitIdel failed!");
			}
		}

		if (_descriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
		}

		if (_commandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(_device, _commandPool, nullptr);
		}

		if (_device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(_device, nullptr);
		}

		if (_vkInstance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(_vkInstance, nullptr);
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	/*
	void RendererVulkan::DestroySwapChain()
	{
		size_t swapChainFramebufferCount = _swapChainFramebuffers.size();
		for (int i = 0; i < swapChainFramebufferCount; ++i)
		{
			vkDestroyFramebuffer(_device, _swapChainFramebuffers[i], nullptr);
		}
		_swapChainFramebuffers.clear();

		size_t swapChainImageViewCount = _swapChainImageViews.size();
		for (int i = 0; i < swapChainImageViewCount; ++i)
		{
			vkDestroyImageView(_device, _swapChainImageViews[i], nullptr);
		}
		_swapChainImageViews.clear();

		if (_renderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, _renderPass, nullptr);
			_renderPass = VK_NULL_HANDLE;
		}

		if (_swapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(_device, _swapChain, nullptr);
			_swapChain = VK_NULL_HANDLE;
		}
	}
	*/

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	VkInstance RendererVulkan::GetVkInstance() const
	{
		return _vkInstance;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	VkDevice RendererVulkan::GetVkDevice() const
	{
		return _device;
	}

	/// @brief 
	/// @return 
	const VkGpuDevice* RendererVulkan::GetVkGpuDevice() const
	{
		return _selectedGpu;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	/*
	VkRenderPass RendererVulkan::GetRenderPass() const
	{
		return _renderPass;
	}
	*/

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	/*
	VkExtent2D RendererVulkan::GetSwapChainExtent() const
	{
		return _swapChainExtent;
	}
	*/

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	VkDescriptorPool RendererVulkan::GetDescriptorPool() const
	{
		return _descriptorPool;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	VkCommandPool RendererVulkan::GetCommandPool() const
	{
		return _commandPool;
	}

	/*
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Material* RendererVulkan::GetSharedMinimalMaterial() const
	{
		return _sharedMinimalMaterial;
	}
	*/

	/// @brief 
	/// @return 
	bool RendererVulkan::Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier)
	{
		_shaderGenerator = DefaultAllocator::GetInstance().New<ShaderGeneratorGLSL>();

		if (CreateVkIntance() == false)
		{
			return false;
		}

		VkSurfaceKHR surface = CreateSurface(mainWindow);
		if (surface == VK_NULL_HANDLE)
		{
			return false;
		}

		EnumeratePhysicalDevice(surface);

		if (SelectPhysicalDevice(physicalDeviceIdentifier) == false)
		{
			return false;
		}

		if (CreateDevice() == false)
		{
			return false;
		}

		if (CreateCommandPool() == false)
		{
			return false;
		}

		VmaAllocatorCreateInfo vmaAllocatorInfo = {};
		vmaAllocatorInfo.physicalDevice = _selectedGpu->physicalDevice;
    	vmaAllocatorInfo.device = _device;
    	vmaAllocatorInfo.instance = _vkInstance;
    	vmaAllocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		if (vmaCreateAllocator(&vmaAllocatorInfo, &_vmaAllocator) != VK_SUCCESS)
		{
			return false;
		}

		_context = DefaultAllocator::GetInstance().New<VkContext>(surface);
		mainWindow->SetSurface(_context);
		/*
		_unlitVertexColorMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteUnlitColor", Material::PolygonMode::Fill, Material::Topololy::TRIANGLE));
		if (_unlitVertexColorMaterial == nullptr)
		{
			return false;
		}
		_unlitVertexColorLineMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteUnlitColor", Material::PolygonMode::Line, Material::Topololy::LINE));
		if (_unlitVertexColorLineMaterial == nullptr)
		{
			return false;
		}
		_sharedMinimalMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteUnlitColor", Material::PolygonMode::Fill, Material::Topololy::TRIANGLE, false));
		if (_sharedMinimalMaterial == nullptr)
		{
			return false;
		}

		_unlitVertexColorMaterialInstance = CreateMaterialInstance(_unlitVertexColorMaterial);
		_unlitVertexColorLineMaterialInstance = CreateMaterialInstance(_unlitVertexColorLineMaterial);
		*/

		_renderQueue->Init();

		return true;
	}

	/// @brief 
	/// @return 
	bool RendererVulkan::CreateVkIntance()
	{
		// === Extensions ===

		Vector<VkExtensionProperties> availableExtensions;
		RendererVulkan::GetAvailableExtensions(availableExtensions);

		std::array<const char*, 0> extensionsRequiredByEngine {

		};

		if (RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredByEngine.data(), extensionsRequiredByEngine.size(), availableExtensions) == false)
		{
			OUTPUT_ERROR("Vulkan: Extensions required by the Engine are not available, try to update 'Vulkan Runtime'");
			return false;
		}

		// TODO move in Context
		std::array<const char*, 2> extensionsRequiredByContext {
			VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(PLATFORM_WINDOWS)
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(PLATFORM_LINUX)
			VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#elif defined(PLATFORM_ANDROID)
			VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#endif
		};

		if (RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredByContext.data(), extensionsRequiredByContext.size(), availableExtensions) == false)
		{
			OUTPUT_ERROR("Vulkan: Extensions required by Context are not available, try to update 'Vulkan Runtime'");
			return false;
		}

		// === Validation Layers ===

#if defined(RENDERER_ENABLE_VALIDATION_LAYER)

		bool enableValidationLayers = true;

		std::array<const char*, 2> extensionsRequiredByValidationLayers {
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};

		if (RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredByValidationLayers.data(), extensionsRequiredByValidationLayers.size(), availableExtensions) == false)
		{
			OUTPUT_ERROR("Vulkan: Extensions required by ValidationLayers are not available, try to update 'Vulkan Runtime'");
			OUTPUT_ERROR("Vulkan: ValidationLayers have been disabled");
			enableValidationLayers = false;
		}

		std::array<const char*, 1> validationLayers {
			//"VK_LAYER_LUNARG_standard_validation"
			"VK_LAYER_KHRONOS_validation"
		};

		if (RendererVulkan::CheckValidationLayerSupport(validationLayers.data(), validationLayers.size()) == false)
		{
			OUTPUT_ERROR("Vulkan: ValidationLayers are not available, try to update 'Vulkan Runtime'");
			OUTPUT_ERROR("Vulkan: ValidationLayers have been disabled");
			enableValidationLayers = false;
		}
#endif

		Vector<const char*> extensions;

#if defined(RENDERER_ENABLE_VALIDATION_LAYER)
		if (enableValidationLayers == true)
		{
			extensions.reserve(extensionsRequiredByEngine.size() + extensionsRequiredByContext.size() + extensionsRequiredByValidationLayers.size());
			extensions.insert(extensions.end(), extensionsRequiredByEngine.begin(), extensionsRequiredByEngine.end());
			extensions.insert(extensions.end(), extensionsRequiredByContext.begin(), extensionsRequiredByContext.end());
			extensions.insert(extensions.end(), extensionsRequiredByValidationLayers.begin(), extensionsRequiredByValidationLayers.end());
		}
		else
#endif
		{
			extensions.reserve(extensionsRequiredByEngine.size() + extensionsRequiredByContext.size());
			extensions.insert(extensions.end(), extensionsRequiredByEngine.begin(), extensionsRequiredByEngine.end());
			extensions.insert(extensions.end(), extensionsRequiredByContext.begin(), extensionsRequiredByContext.end());
		}

		// === Create Instance ===

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello"; // TODO ?
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // TODO ?
		appInfo.pEngineName = "HodEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // TODO ?
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

#if defined(RENDERER_ENABLE_VALIDATION_LAYER)
		if (enableValidationLayers == true)
		{
			VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
			debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
													   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
													   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
													   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
												   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
												   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
#if defined(PLATFORM_WINDOWS)
												   | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
#endif
												   ;
			debugMessengerCreateInfo.pfnUserCallback = &RendererVulkan::DebugCallback;
			debugMessengerCreateInfo.pUserData = nullptr;

			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			createInfo.pNext = &debugMessengerCreateInfo;
		}
		else
#endif
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &_vkInstance) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create a Vulkan instance !");
			return false;
		}

		return true;
	}

	/// @brief 
	/// @param availableExtensions 
	void RendererVulkan::GetAvailableExtensions(Vector<VkExtensionProperties>& availableExtensions)
	{
		uint32_t availableExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

		availableExtensions.resize(availableExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());
	}

	/// @brief 
	/// @param extensions 
	/// @param extensionCount 
	/// @param availableExtensions 
	/// @return 
	bool RendererVulkan::CheckExtensionsIsAvailable(const char** extensions, size_t extensionCount, const Vector<VkExtensionProperties>& availableExtensions)
	{
		size_t availableExtensionsCount = availableExtensions.size();

		for (size_t i = 0; i < extensionCount; ++i)
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
			{
				return false;
			}
		}

		return true;
	}

#if defined(RENDERER_ENABLE_VALIDATION_LAYER)
	/// @brief 
	/// @param validationLayers 
	/// @param validationLayerCount 
	/// @return 
	bool RendererVulkan::CheckValidationLayerSupport(const char** validationLayers, size_t validationLayerCount)
	{
		uint32_t availableValidationLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, nullptr);

		Vector<VkLayerProperties> availableValidationLayers(availableValidationLayerCount);
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, availableValidationLayers.data());

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

	/// @brief 
	/// @param messageSeverity 
	/// @param messageType 
	/// @param pCallbackData 
	/// @param pUserData 
	/// @return 
	VKAPI_ATTR VkBool32 VKAPI_CALL RendererVulkan::DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		(void)messageType;
		(void)pUserData;

		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			OUTPUT_ERROR("Validation Layer: {}", pCallbackData->pMessage);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			OUTPUT_WARNING("Validation Layer: {}", pCallbackData->pMessage);
		}
		else
		{
			OUTPUT_MESSAGE("Validation Layer: {}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}
#endif

#if defined(PLATFORM_WINDOWS)
	/// @brief 
	/// @param window 
	/// @return 
	VkSurfaceKHR RendererVulkan::CreateSurface(window::Window* window)
	{
		window::Win32Window* win32Window = static_cast<window::Win32Window*>(window);

		VkWin32SurfaceCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.flags = 0;
		createInfo.hwnd = win32Window->GetWindowHandle();
		createInfo.hinstance = win32Window->GetInstanceHandle();
		createInfo.pNext = nullptr;

		VkSurfaceKHR surface;
		if (vkCreateWin32SurfaceKHR(_vkInstance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create Win32 Surface !");
			return nullptr;
		}

		return surface;
	}
#elif defined(PLATFORM_LINUX)
	/// @brief 
	/// @param window 
	/// @return 
	VkSurfaceKHR RendererVulkan::CreateSurface(window::Window* window)
	{
		window::WaylandWindow* waylandWindow = static_cast<window::WaylandWindow*>(window);
		window::WaylandDisplayManager* waylandDisplayManager = window::WaylandDisplayManager::GetInstance();

		VkWaylandSurfaceCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.flags = 0;
		createInfo.display = waylandDisplayManager->GetDisplay();
		createInfo.surface = waylandWindow->GetWaylandSurface();
		createInfo.pNext = nullptr;

		VkSurfaceKHR surface;
		if (vkCreateWaylandSurfaceKHR(_vkInstance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create Wayland Surface !");
			return nullptr;
		}

		return surface;
	}
#elif defined(PLATFORM_ANDROID)
	/// @brief 
	/// @param window 
	/// @return 
	VkSurfaceKHR RendererVulkan::CreateSurface(window::Window* window)
	{
		window::AndroidWindow* androidWindow = static_cast<window::AndroidWindow*>(window);

        if (window == nullptr)
        {
            OUTPUT_ERROR("Vulkan: Unable to create Android Surface, Windows is nullptr");
            return nullptr;
        }

		VkAndroidSurfaceCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		createInfo.flags = 0;
		createInfo.window = androidWindow->GetNativeWindow();
		createInfo.pNext = nullptr;

		VkSurfaceKHR surface;
		if (vkCreateAndroidSurfaceKHR(_vkInstance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create Android Surface !");
			return nullptr;
		}

		return surface;
	}
#endif

	/// @brief 
	/// @param window 
	/// @return 
	bool RendererVulkan::CreateContext(window::Window* window)
	{
		VkSurfaceKHR surface = CreateSurface(window);
		if (surface == VK_NULL_HANDLE)
		{
			return false;
		}

		window->SetSurface(DefaultAllocator::GetInstance().New<VkContext>(surface));
		return true;
	}

	/// @brief 
	/// @param context 
	/// @param physicalDeviceIdentifier 
	/// @return 
	bool RendererVulkan::BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier)
	{
		EnumeratePhysicalDevice(static_cast<VkContext*>(context)->GetSurface());
		if (SelectPhysicalDevice(physicalDeviceIdentifier) == false)
		{
			return false;
		}

		if (CreateDevice() == false)
		{
			return false;
		}

		if (CreateCommandPool() == false)
		{
			return false;
		}

		_context = (VkContext*)context;

		/*
		_unlitVertexColorMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteUnlitColor", Material::PolygonMode::Fill, Material::Topololy::TRIANGLE));
		_unlitVertexColorLineMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteUnlitColor", Material::PolygonMode::Line, Material::Topololy::LINE));
		_sharedMinimalMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteUnlitColor", Material::PolygonMode::Fill, Material::Topololy::TRIANGLE, false));

		_unlitVertexColorMaterialInstance = CreateMaterialInstance(_unlitVertexColorMaterial);
		_unlitVertexColorLineMaterialInstance = CreateMaterialInstance(_unlitVertexColorLineMaterial);
		*/

		return true;
	}

	/// @brief 
	/// @param availableDevices 
	/// @return 
	bool RendererVulkan::GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices)
	{
		return false; // TODO
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void RendererVulkan::EnumeratePhysicalDevice(VkSurfaceKHR surface)
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, nullptr);

		Vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(_vkInstance, &physicalDeviceCount, physicalDevices.data());

		_availableGpu.resize(physicalDeviceCount);

		_recommandedGpu = nullptr;
		for (size_t i = 0; i < physicalDeviceCount; ++i)
		{
			VkPhysicalDevice physicalDevice = physicalDevices[i];
			VkGpuDevice& gpuDevice = _availableGpu[i];

			FillPhysicalDeviceInfo(surface, physicalDevice, gpuDevice);

			if (gpuDevice.compatible == true)
			{
				if (_recommandedGpu == nullptr)
				{
					_recommandedGpu = &gpuDevice;
				}
				else if (gpuDevice.score > _recommandedGpu->score)
				{
					_recommandedGpu = &gpuDevice;
				}
			}
		}
	}

	/// @brief 
	/// @param physicalDevice 
	/// @param gpuDevice 
	/// @return 
	void RendererVulkan::FillPhysicalDeviceInfo(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkGpuDevice& gpuDevice)
	{
		gpuDevice.physicalDevice = physicalDevice;
		gpuDevice.compatible = false;
		gpuDevice.score = 0;

		vkGetPhysicalDeviceProperties(physicalDevice, &gpuDevice.deviceProperties);

		gpuDevice.name = std::wstring(gpuDevice.deviceProperties.deviceName, gpuDevice.deviceProperties.deviceName + strlen(gpuDevice.deviceProperties.deviceName));
		gpuDevice.score += gpuDevice.deviceProperties.limits.maxImageDimension2D;

		if (gpuDevice.deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			gpuDevice.score += 1000000;
		}

		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &gpuDevice.memProperties);

		size_t vram = 0;

		for (size_t i = 0; i < gpuDevice.memProperties.memoryHeapCount; ++i)
		{
			const VkMemoryHeap& heap = gpuDevice.memProperties.memoryHeaps[i];

			if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				if (heap.size > vram)
				{
					vram = heap.size / 1000000;
				}
			}
		}

		gpuDevice.vram = vram;
		gpuDevice.score += vram;

		gpuDevice.graphicsAndPresentQueueFamilyIndex = 0;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		for (size_t i = 0; i < queueFamilyCount; ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				VkBool32 presentSupport = VK_FALSE;
				if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, (uint32_t)i, surface, &presentSupport) != VK_SUCCESS)
				{
					OUTPUT_ERROR("Vulkan: Unable to check present support on FamilyQueue !");
					gpuDevice.graphicsAndPresentQueueFamilyIndex = 0;
					gpuDevice.compatible = false;
				}
				else if (presentSupport == VK_TRUE)
				{
					gpuDevice.graphicsAndPresentQueueFamilyIndex = (uint32_t)i;
					gpuDevice.compatible = true;
					break;
				}
			}
		}

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		if (deviceFeatures.geometryShader == VK_FALSE)
		{
			gpuDevice.compatible = false;
			return;
		}

		std::array<const char*, 1> requiredExtensions {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		uint32_t availableExtensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, nullptr);

		Vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, availableExtensions.data());

		if (RendererVulkan::CheckExtensionsIsAvailable(requiredExtensions.data(), requiredExtensions.size(), availableExtensions) == false)
		{
			gpuDevice.compatible = false;
			return;
		}

		VkSurfaceCapabilitiesKHR capabilities;
		Vector<VkSurfaceFormatKHR> formats;
		Vector<VkPresentModeKHR> presentModes;

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to get Surface capabilities !");
			gpuDevice.compatible = false;
			return;
		}
		else
		{
			uint32_t formatCount;
			if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to get Surface formats !");
				gpuDevice.compatible = false;
				return;
			}

			if (formatCount != 0)
			{
				formats.resize(formatCount);
				if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data()) != VK_SUCCESS)
				{
					OUTPUT_ERROR("Vulkan: Unable to get Surface formats !");
					gpuDevice.compatible = false;
					return;
				}

				bool targetFormatFounded = false;

				for (size_t j = 0; j < formatCount; ++j)
				{
					const VkSurfaceFormatKHR& format = formats[j];

					//if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					{
						targetFormatFounded = true;
						break;
					}
				}

				if (targetFormatFounded == false)
				{
					gpuDevice.compatible = false;
					return;
				}
			}
			else
			{
				OUTPUT_ERROR("Vulkan: No Surface formats !");
				gpuDevice.compatible = false;
				return;
			}

			uint32_t presentModeCount;
			if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to get Surface present modes !");
				gpuDevice.compatible = false;
				return;
			}

			if (presentModeCount != 0)
			{
				presentModes.resize(presentModeCount);
				if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()) != VK_SUCCESS)
				{
					OUTPUT_ERROR("Vulkan: Unable to get Surface present modes !");
					gpuDevice.compatible = false;
					return;
				}
			}
			else
			{
				OUTPUT_ERROR("Vulkan: No Surface present modes !");
				gpuDevice.compatible = false;
				return;
			}
		}
	}

	/// @brief 
	/// @param physicalDeviceIdentifier 
	/// @return 
	bool RendererVulkan::SelectPhysicalDevice(uint32_t physicalDeviceIdentifier)
	{
		_selectedGpu = nullptr;

		if (physicalDeviceIdentifier != 0)
		{
			for (const VkGpuDevice& gpu : _availableGpu)
			{
				if (gpu.deviceProperties.deviceID == physicalDeviceIdentifier)
				{
					_selectedGpu = &gpu;
					break;
				}
			}
			if (_selectedGpu == nullptr)
			{
				OUTPUT_ERROR("Vulkan: PhysicalDevice {} not found, fallback on recommanded physical device", physicalDeviceIdentifier);
				_selectedGpu = _recommandedGpu;
			}
			else if (_selectedGpu->compatible == false)
			{
				OUTPUT_ERROR("Vulkan: Selected PhysicalDevice {} is not compatible with GameEngine requirement, fallback on recommanded physical device", physicalDeviceIdentifier);
				_selectedGpu = _recommandedGpu;
			}
		}
		else
		{
			_selectedGpu = _recommandedGpu;
		}

		if (_selectedGpu == nullptr)
		{
			OUTPUT_ERROR("Vulkan: No PhysicalDevice compatible with GameEngine requirement... sorry");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::CreateDevice()
	{
		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = _selectedGpu->graphicsAndPresentQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.fillModeNonSolid = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		const Vector<const char*> requiredExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		// Deprecated, now _device share Validation Layer with the VkInstance
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(_selectedGpu->physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create logical device !");
			return false;
		}

		vkGetDeviceQueue(_device, _selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, _selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &_presentQueue);

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	/*
	bool RendererVulkan::CreateSwapChain()
	{
		vkDeviceWaitIdle(_device);

		DestroySwapChain();

		VkSurfaceCapabilitiesKHR capabilities;
		Vector<VkSurfaceFormatKHR> formats;
		Vector<VkPresentModeKHR> presentModes;

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_selectedGpu->physicalDevice, _surface, &capabilities) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to get Surface capabilities !");
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

		_swapChainExtent = extent;

		uint32_t imageCount = capabilities.minImageCount + 1;

		if (imageCount > capabilities.maxImageCount)
			imageCount = capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = _swapChainExtent;
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

		if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create SwapChain !");
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

		if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create render pass!");
			return false;
		}

		if (_depthTexture.BuildDepth(extent.width, extent.height) == false)
			return false;

		// Image views
		vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);

		Vector<VkImage> swapChainImages(imageCount);
		vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, swapChainImages.data());

		_swapChainImageViews.resize(imageCount, VK_NULL_HANDLE);
		for (size_t i = 0; i < imageCount; ++i)
		{
			TransitionImageLayout(swapChainImages[i], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

			// TODO use CreateImageView here ?

			VkImageViewCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageCreateInfo.image = swapChainImages[i];
			imageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
			imageCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCreateInfo.subresourceRange.baseMipLevel = 0;
			imageCreateInfo.subresourceRange.levelCount = 1;
			imageCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(_device, &imageCreateInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create Image Views !");
				return false;
			}
		}

		// Framebuffers
		_swapChainFramebuffers.resize(imageCount);
		for (size_t i = 0; i < imageCount; i++)
		{
			VkImageView attachments[] = {
				_swapChainImageViews[i],
				_depthTexture.GetTextureImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = 2;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = _swapChainExtent.width;
			framebufferInfo.height = _swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create Framebuffer !");
				return false;
			}
		}

		return true;
	}
	*/

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = _selectedGpu->graphicsAndPresentQueueFamilyIndex;
		poolInfo.flags = 0; // Optional

		if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create command pool!");
			return false;
		}

		VkDescriptorPoolSize poolSizes[4];

		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 150000; // TODO

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 150; // TODO

		poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes[2].descriptorCount = 150; // TODO

		poolSizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes[3].descriptorCount = 150; // TODO

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = 4;
		descriptorPoolInfo.pPoolSizes = poolSizes;
		descriptorPoolInfo.maxSets = 150000; // TODO
		descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		if (vkCreateDescriptorPool(_device, &descriptorPoolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create descriptor pool!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = bufferUsage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device, &bufferInfo, nullptr, buffer) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create buffer!");
			return false;
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device, *buffer, &memRequirements);

		uint32_t memoryTypeIndex = 0;
		if (FindMemoryTypeIndex(memRequirements.memoryTypeBits, memoryProperties, &memoryTypeIndex) == false)
		{
			OUTPUT_ERROR("Vulkan: Unable to find memory type for this buffer!");
			return false;
		}

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(_device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to allocate buffer memory!");
			return false;
		}

		if (vkBindBufferMemory(_device, *buffer, *bufferMemory, 0) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to bind buffer and buffer memory!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
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

		if (vkCreateImage(_device, &imageInfo, nullptr, image) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to create image!");
			return false;
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, *image, &memRequirements);

		uint32_t memoryTypeIndex = 0;

		if (FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties, &memoryTypeIndex) == false)
		{
			return false;
		}

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(_device, &allocInfo, nullptr, imageMemory) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to allocate image memory!");
			return false;
		}

		if (vkBindImageMemory(_device, *image, *imageMemory, 0) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to bind image and image memory!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
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

		if (vkCreateImageView(_device, &viewInfo, nullptr, imageView) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Failed to create texture image view!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::CreateSampler(VkSampler* sampler, const SamplerCreateInfo& createInfo)
	{
		*sampler = VK_NULL_HANDLE;

		VkSamplerAddressMode addressMode = createInfo._wrapMode == WrapMode::Clamp ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkFilter filter = createInfo._filterMode == FilterMode::Linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		VkSamplerMipmapMode mipmapMode = createInfo._filterMode == FilterMode::Linear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = filter;
		samplerInfo.minFilter = filter;
		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = mipmapMode;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(_device, &samplerInfo, nullptr, sampler) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Failed to create texture sampler!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::BeginSingleTimeCommands(VkCommandBuffer* commandBuffer)
	{
		*commandBuffer = VK_NULL_HANDLE;

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(_device, &allocInfo, commandBuffer) != VK_SUCCESS)
			return false;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			vkFreeCommandBuffers(_device, _commandPool, 1, commandBuffer);
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		bool ret = false;
		VkSubmitInfo submitInfo = {};

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			goto exit;

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			goto exit;

		if (vkQueueWaitIdle(_graphicsQueue) != VK_SUCCESS)
			goto exit;

		ret = true;

	exit:
		vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);

		return ret;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkBufferCopy copyRegion = {};
		copyRegion.size = size;

		if (BeginSingleTimeCommands(&commandBuffer) == false)
		{
			return false;
		}

		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		if (EndSingleTimeCommands(commandBuffer) == false)
		{
			return false;
		}

		return true;
	}

	/// @brief 
	/// @param commandBuffer 
	/// @param image 
	/// @param aspectFlags 
	/// @param oldLayout 
	/// @param newLayout 
	/// @return 
	bool RendererVulkan::TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
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
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			OUTPUT_ERROR("Vulkan: TransitionImageLayout, unsupported layout transition!");
			return false;
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		return true;
	}

	/// @brief 
	/// @param image 
	/// @param aspectFlags 
	/// @param oldLayout 
	/// @param newLayout 
	/// @return 
	bool RendererVulkan::TransitionImageLayoutImmediate(VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

		if (BeginSingleTimeCommands(&commandBuffer) == false)
		{
			return false;
		}

		TransitionImageLayout(commandBuffer, image, aspectFlags, oldLayout, newLayout);

		if (EndSingleTimeCommands(commandBuffer) == false)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

		if (BeginSingleTimeCommands(&commandBuffer) == false)
		{
			return false;
		}

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		if (EndSingleTimeCommands(commandBuffer) == false)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::CopyImageToBuffer(VkImage image, VkBuffer buffer, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

		if (BeginSingleTimeCommands(&commandBuffer) == false)
		{
			return false;
		}

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = { width, height, 1};

		vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region);

		if (EndSingleTimeCommands(commandBuffer) == false)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererVulkan::FindMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperties, uint32_t* memoryTypeIndex)
	{
		for (uint32_t i = 0; i < _selectedGpu->memProperties.memoryTypeCount; ++i)
		{
			if ((memoryTypeBits & (1 << i)) && (_selectedGpu->memProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
			{
				*memoryTypeIndex = i;
				return true;
			}
		}

		return false;
	}

	/// @brief 
	/// @param commandBuffers 
	/// @param commandBufferCount 
	/// @param signalSemaphore 
	/// @param waitSemaphore 
	/// @param fence 
	/// @return 
	bool RendererVulkan::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore, const Fence* fence)
	{
		Vector<VkCommandBuffer> vkCommandBuffers(commandBufferCount);
		for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
		{
			vkCommandBuffers[commandBufferIndex] = static_cast<CommandBufferVk*>(commandBuffers[commandBufferIndex])->GetVkCommandBuffer();
		}

		VkSemaphore vkSignalSemaphore = VK_NULL_HANDLE;
		VkSemaphore vkWaitSemaphore = VK_NULL_HANDLE;
		VkFence vkFence = VK_NULL_HANDLE;
		if (fence != nullptr)
		{
			vkFence = static_cast<const FenceVk*>(fence)->GetVkFence();
		}

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		if (signalSemaphore != nullptr)
		{
			vkSignalSemaphore = static_cast<const SemaphoreVk*>(signalSemaphore)->GetVkSemaphore();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &vkSignalSemaphore;
		}
		else
		{
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
		}
		if (waitSemaphore != nullptr)
		{
			vkWaitSemaphore = static_cast<const SemaphoreVk*>(waitSemaphore)->GetVkSemaphore();
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &vkWaitSemaphore;
			submitInfo.pWaitDstStageMask = waitStages;
		}
		else
		{
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
		}
		
		submitInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.size());
		submitInfo.pCommandBuffers = vkCommandBuffers.data();

		if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, vkFence) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to submit draw command buffer!");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Shader* RendererVulkan::CreateShader(Shader::ShaderType type)
	{
		return DefaultAllocator::GetInstance().New<VkShader>(type);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Material* RendererVulkan::CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
	{
		VkMaterial* mat = DefaultAllocator::GetInstance().New<VkMaterial>();

		if (mat->Build(vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
		{
			DefaultAllocator::GetInstance().Delete(mat);
			return nullptr;
		}

		return mat;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	MaterialInstance* RendererVulkan::CreateMaterialInstance(const Material* material)
	{
		if (material == nullptr)
		{
			// todo message, why not use ref ?
			return nullptr;
		}

		return DefaultAllocator::GetInstance().New<VkMaterialInstance>(*material);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	CommandBuffer* RendererVulkan::CreateCommandBuffer()
	{
		return DefaultAllocator::GetInstance().New<CommandBufferVk>();
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Buffer* RendererVulkan::CreateBuffer(Buffer::Usage usage, uint32_t size)
	{
		return DefaultAllocator::GetInstance().New<BufferVk>(usage, size);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Texture* RendererVulkan::CreateTexture()
	{
		return DefaultAllocator::GetInstance().New<VkTexture>();
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	RenderTarget* RendererVulkan::CreateRenderTarget()
	{
		return DefaultAllocator::GetInstance().New<VkRenderTarget>();
	}

	/// @brief 
	/// @return 
	Semaphore* RendererVulkan::CreateSemaphore()
	{
		return DefaultAllocator::GetInstance().New<SemaphoreVk>();
	}

	/// @brief 
	/// @return 
	Fence* RendererVulkan::CreateFence()
	{
		return DefaultAllocator::GetInstance().New<FenceVk>();
	}
}
