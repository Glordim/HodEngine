#include "RendererVulkan.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/glm.hpp"

#include "../../RenderQueue.h"

#include "BufferVk.h"
#include "CommandBufferVk.h"
#include "VkTexture.h"
#include "VkShader.h"
#include "VkMaterial.h"
#include "VkMaterialInstance.h"

#include <Application/src/Application.h>

#include <Core/Src/Output.h>

#include <ImGui/src/imgui.h>
#include <ImGui/src/imgui_impl_vulkan.h>

#include <SPIRV-Cross/spirv_cross.hpp>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererVulkan::~RendererVulkan()
		{
			if (_device != VK_NULL_HANDLE)
			{
				if (vkDeviceWaitIdle(_device) != VK_SUCCESS)
					OUTPUT_ERROR("Vulkan: DeviceWaitIdel failed!\n");
			}

			if (_acquireNextImageFence != VK_NULL_HANDLE)
				vkDestroyFence(_device, _acquireNextImageFence, nullptr);

			if (_imageAvailableSemaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);

			if (_renderFinishedSemaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);

			if (_descriptorPool != VK_NULL_HANDLE)
				vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);

			if (_commandPool != VK_NULL_HANDLE)
				vkDestroyCommandPool(_device, _commandPool, nullptr);

			DestroySwapChain();

			if (_device != VK_NULL_HANDLE)
				vkDestroyDevice(_device, nullptr);

			if (_surface != VK_NULL_HANDLE)
				vkDestroySurfaceKHR(_instance, _surface, nullptr);

			if (_instance != VK_NULL_HANDLE)
				vkDestroyInstance(_instance, nullptr);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkInstance RendererVulkan::GetVkInstance() const
		{
			return _instance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkDevice RendererVulkan::GetVkDevice() const
		{
			return _device;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkRenderPass RendererVulkan::GetRenderPass() const
		{
			return _renderPass;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkExtent2D RendererVulkan::GetSwapChainExtent() const
		{
			return _swapChainExtent;
		}

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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkFramebuffer RendererVulkan::GetSwapChainCurrentFrameBuffer()
		{
			return _swapChainFramebuffers[_currentImageIndex];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material* RendererVulkan::GetSharedMinimalMaterial() const
		{
			return _sharedMinimalMaterial;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::Init(APPLICATION::Application* pApplication, bool enableValidationLayers)
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

			if (pApplication->GetExtensionRequiredToCreateVulkanSurface(&extensionsRequiredBySDL) == false)
				return false;

			std::vector<VkExtensionProperties> availableExtensions;

			RendererVulkan::GetAvailableExtensions(&availableExtensions);

			if (RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredByEngine, availableExtensions) == false)
			{
				OUTPUT_ERROR("Vulkan: Extensions required by the Engine are not available, try to update 'Vulkan Runtime'\n");
				return false;
			}

			if (RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredBySDL, availableExtensions) == false)
			{
				OUTPUT_ERROR("Vulkan: Extensions required by SDL are not available, try to update 'Vulkan Runtime'\n");
				return false;
			}

			if (enableValidationLayers == true && RendererVulkan::CheckExtensionsIsAvailable(extensionsRequiredByValidationLayers, availableExtensions) == false)
			{
				OUTPUT_ERROR("Vulkan: Extensions required by ValidationLayers are not available, try to update 'Vulkan Runtime'\n");
				OUTPUT_ERROR("Vulkan: ValidationLayers have been disabled\n");
				enableValidationLayers = false;
			}

			// === Validation Layers ===

			const std::vector<const char*> validationLayers = {
				"VK_LAYER_LUNARG_standard_validation"
				//"VK_LAYER_KHRONOS_validation"
			};

			if (enableValidationLayers == true && RendererVulkan::CheckValidationLayerSupport(validationLayers) == false)
			{
				OUTPUT_ERROR("Vulkan: ValidationLayers are not available, try to update 'Vulkan Runtime'\n");
				OUTPUT_ERROR("Vulkan: ValidationLayers have been disabled\n");
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

			if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create a Vulkan instance !\n");
				return false;
			}

			if (pApplication->CreateVulkanSurface(_instance, &_surface) == false)
			{
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RendererVulkan::GetAvailableExtensions(std::vector<VkExtensionProperties>* availableExtensions)
		{
			uint32_t availableExtensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

			availableExtensions->resize(availableExtensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions->data());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VKAPI_ATTR VkBool32 VKAPI_CALL RendererVulkan::DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData)
		{
			(void)messageSeverity;
			(void)messageType;
			(void)pUserData;

			if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				OUTPUT_ERROR("Validation Layer: %s", pCallbackData->pMessage);
			}
			else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				OUTPUT_WARNING("Validation Layer: %s", pCallbackData->pMessage);
			}
			else
			{
				OUTPUT_MESSAGE("Validation Layer: %s", pCallbackData->pMessage);
			}

			return VK_FALSE;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices)
		{
			if (availableDevices == nullptr)
				return false;

			if (_availableGpu.empty() == true)
			{
				uint32_t physicalDeviceCount = 0;
				vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);

				if (physicalDeviceCount == 0)
					return false;

				_availableGpu.resize(physicalDeviceCount);

				std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
				vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data());

				for (size_t i = 0; i < physicalDeviceCount; ++i)
				{
					const VkPhysicalDevice& physicalDevice = physicalDevices[i];

					VkPhysicalDeviceProperties deviceProperties;
					VkPhysicalDeviceFeatures deviceFeatures;
					vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
					vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

					VkGpuDevice& device = _availableGpu[i];
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
							if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, (uint32_t)j, _surface, &presentSupport) != VK_SUCCESS)
							{
								OUTPUT_ERROR("Vulkan: Unable to check present support on FamilyQueue !\n");
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

						if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _surface, &capabilities) != VK_SUCCESS)
						{
							OUTPUT_ERROR("Vulkan: Unable to get Surface capabilities !\n");
							device.compatible = false;
						}
						else
						{
							uint32_t formatCount;
							if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, nullptr) != VK_SUCCESS)
							{
								OUTPUT_ERROR("Vulkan: Unable to get Surface formats !\n");
								device.compatible = false;
							}

							if (formatCount != 0)
							{
								formats.resize(formatCount);
								if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, formats.data()) != VK_SUCCESS)
								{
									OUTPUT_ERROR("Vulkan: Unable to get Surface formats !\n");
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
								OUTPUT_ERROR("Vulkan: No Surface formats !\n");
								device.compatible = false;
							}

							uint32_t presentModeCount;
							if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &presentModeCount, nullptr) != VK_SUCCESS)
							{
								OUTPUT_ERROR("Vulkan: Unable to get Surface present modes !\n");
								device.compatible = false;
							}

							if (presentModeCount != 0)
							{
								presentModes.resize(presentModeCount);
								if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &presentModeCount, presentModes.data()) != VK_SUCCESS)
								{
									OUTPUT_ERROR("Vulkan: Unable to get Surface present modes !\n");
									device.compatible = false;
								}
							}
							else
							{
								OUTPUT_ERROR("Vulkan: No Surface present modes !\n");
								device.compatible = false;
							}
						}
					}
				}
			}

			size_t avalaibleDeviceCount = _availableGpu.size();
			availableDevices->resize(avalaibleDeviceCount);

			for (size_t i = 0; i < avalaibleDeviceCount; ++i)
			{
				availableDevices->at(i) = (GpuDevice*)(&_availableGpu[i]);
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::BuildPipeline(GpuDevice* physicalDevice)
		{
			_selectedGpu = (VkGpuDevice*)physicalDevice;

			if (CreateDevice() == false)
			{
				return false;
			}

			if (CreateCommandPool() == false)
			{
				return false;
			}

			if (CreateSwapChain() == false)
			{
				return false;
			}

			if (CreateSemaphores() == false)
			{
				return false;
			}

			/*
			Shader* vertexShader = CreateShader("Shader/SpriteUnlitColor.vert.spirv", Shader::Vertex);
			if (vertexShader == nullptr)
			{
				return false;*
			}

			Shader* fragmentShader = CreateShader("Shader/SpriteUnlitColor.frag.spirv", Shader::Fragment);
			if (fragmentShader == nullptr)
			{
				return false;
			}

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
					_viewLayout.ExtractBlockUbo(compVert, resource);
				}
				else if (set == 1)
				{
					_modelLayout.ExtractBlockUbo(compVert, resource);
				}
			}

			size_t textureCount = resourcesVert.sampled_images.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesVert.sampled_images[i];

				size_t set = compVert.get_decoration(resource.id, spv::DecorationDescriptorSet);

				if (set == 0)
				{
					_viewLayout.ExtractBlockTexture(compVert, resource);
				}
				else if (set == 1)
				{
					_modelLayout.ExtractBlockTexture(compVert, resource);
				}
			}

			spirv_cross::Compiler compFrag(((VkShader*)fragmentShader)->GetShaderBytecode());
			spirv_cross::ShaderResources resourcesFrag = compFrag.get_shader_resources();

			uniformBufferCount = resourcesFrag.uniform_buffers.size();
			for (size_t i = 0; i < uniformBufferCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesFrag.uniform_buffers[i];

				size_t set = compFrag.get_decoration(resource.id, spv::DecorationDescriptorSet);

				if (set == 0)
				{
					_viewLayout.ExtractBlockUbo(compFrag, resource);
				}
				else if (set == 1)
				{
					_modelLayout.ExtractBlockUbo(compFrag, resource);
				}
			}

			textureCount = resourcesFrag.sampled_images.size();
			for (size_t i = 0; i < textureCount; ++i)
			{
				spirv_cross::Resource& resource = resourcesFrag.sampled_images[i];

				size_t set = compFrag.get_decoration(resource.id, spv::DecorationDescriptorSet);

				if (set == 0)
				{
					_viewLayout.ExtractBlockTexture(compFrag, resource);
				}
				else if (set == 1)
				{
					_modelLayout.ExtractBlockTexture(compFrag, resource);
				}
			}
			
			_viewLayout.BuildDescriptorSetLayout();
			_modelLayout.BuildDescriptorSetLayout();

			_unlitVertexColorMaterial = CreateMaterial(vertexShader, fragmentShader, Material::PolygonMode::Fill, Material::Topololy::TRIANGLE);
			_unlitVertexColorMaterialInstance = CreateMaterialInstance(_unlitVertexColorMaterial);

			_unlitVertexColorLineMaterial = CreateMaterial(vertexShader, fragmentShader, Material::PolygonMode::Line, Material::Topololy::LINE);
			_unlitVertexColorLineMaterialInstance = CreateMaterialInstance(_unlitVertexColorLineMaterial);

			_sharedMinimalMaterial = CreateMaterial(vertexShader, fragmentShader, Material::PolygonMode::Fill, Material::Topololy::TRIANGLE, false);
			*/

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

			const std::vector<const char*> requiredExtensions = {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
			createInfo.ppEnabledExtensionNames = requiredExtensions.data();

			// Deprecated, now _device share Validation Layer with the VkInstance
			createInfo.ppEnabledLayerNames = nullptr;
			createInfo.enabledLayerCount = 0;

			if (vkCreateDevice(_selectedGpu->physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create logical device !\n");
				return false;
			}

			vkGetDeviceQueue(_device, _selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &_graphicsQueue);
			vkGetDeviceQueue(_device, _selectedGpu->graphicsAndPresentQueueFamilyIndex, 0, &_presentQueue);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkDescriptorSetLayout RendererVulkan::GetVkViewDescriptorSet() const
		{
			return _viewLayout.GetDescriptorSetLayout();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		VkDescriptorSetLayout RendererVulkan::GetVkModelDescriptorSet() const
		{
			return _modelLayout.GetDescriptorSetLayout();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::CreateSwapChain()
		{
			vkDeviceWaitIdle(_device);

			DestroySwapChain();

			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;

			if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_selectedGpu->physicalDevice, _surface, &capabilities) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to get Surface capabilities !\n");
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
				OUTPUT_ERROR("Vulkan: Unable to create SwapChain !\n");
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

			std::vector<VkImage> swapChainImages(imageCount);
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
				OUTPUT_ERROR("Vulkan: Unable to create command pool!\n");
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

			if (vkCreateDescriptorPool(_device, &descriptorPoolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create descriptor pool!\n");
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::CreateSemaphores()
		{
			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create semaphores!\n");
				return false;
			}

			if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create semaphores!\n");
				return false;
			}

			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = 0;
			fenceCreateInfo.pNext = nullptr;

			if (vkCreateFence(_device, &fenceCreateInfo, nullptr, &_acquireNextImageFence) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create fence!\n");
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
				OUTPUT_ERROR("Vulkan: Unable to create buffer!\n");
				return false;
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(_device, *buffer, &memRequirements);

			uint32_t memoryTypeIndex = 0;
			if (FindMemoryTypeIndex(memRequirements.memoryTypeBits, memoryProperties, &memoryTypeIndex) == false)
			{
				OUTPUT_ERROR("Vulkan: Unable to find memory type for this buffer!\n");
				return false;
			}

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = memoryTypeIndex;

			if (vkAllocateMemory(_device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to allocate buffer memory!\n");
				return false;
			}

			if (vkBindBufferMemory(_device, *buffer, *bufferMemory, 0) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to bind buffer and buffer memory!\n");
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
				OUTPUT_ERROR("Vulkan: Unable to create image!\n");
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
				OUTPUT_ERROR("Vulkan: Unable to allocate image memory!\n");
				return false;
			}

			if (vkBindImageMemory(_device, *image, *imageMemory, 0) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to bind image and image memory!\n");
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
				OUTPUT_ERROR("Vulkan: Failed to create texture image view!\n");
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

			if (vkCreateSampler(_device, &samplerInfo, nullptr, sampler) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Failed to create texture sampler!\n");
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
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
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = 0;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else
			{
				OUTPUT_ERROR("Vulkan: TransitionImageLayout, unsupported layout transition!\n");
				return false;
			}

			VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

			if (BeginSingleTimeCommands(&commandBuffer) == false)
			{
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
			region.imageExtent = {
				width,
				height,
				1
			};

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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::ResizeSwapChain()
		{
			return CreateSwapChain();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::AcquireNextImageIndex()
		{
			if (_swapChain == nullptr) // For exemple if the window is hidden the size will be 0 and not swap chain are created
				return false;

			if (vkResetFences(_device, 1, &_acquireNextImageFence) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to reset fence!\n");
				return false;
			}

			VkResult result = vkAcquireNextImageKHR(_device, _swapChain, std::numeric_limits<uint64_t>::max(), VK_NULL_HANDLE, _acquireNextImageFence, &_currentImageIndex);
			if (result != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to acquire next image!\n");

				if (result == VK_ERROR_OUT_OF_DATE_KHR)
				{
					OUTPUT_ERROR("Vulkan: VK_ERROR_OUT_OF_DATE_KHR recreating SwapChain...\n");
					ResizeSwapChain();
				}
				else if (result == VK_SUBOPTIMAL_KHR)
				{
					OUTPUT_ERROR("Vulkan: VK_SUBOPTIMAL_KHR recreating SwapChain...\n");
					ResizeSwapChain();
				}

				return false;
			}

			if (vkWaitForFences(_device, 1, &_acquireNextImageFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to wait fence\n");
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount)
		{
			std::vector<VkCommandBuffer> vkCommandBuffers(commandBufferCount);
			for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
			{
				vkCommandBuffers[commandBufferIndex] = static_cast<CommandBufferVk*>(commandBuffers[commandBufferIndex])->GetVkCommandBuffer();
			}

			VkFence fence = VK_NULL_HANDLE;

			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = 0;
			fenceCreateInfo.pNext = nullptr;

			if (vkCreateFence(_device, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to create Fence!");
				return false;
			}

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.size());
			submitInfo.pCommandBuffers = vkCommandBuffers.data();

			if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to submit draw command buffer!");
				vkDestroyFence(_device, fence, nullptr);
				return false;
			}

			if (vkWaitForFences(_device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to wait fence");
				vkDestroyFence(_device, fence, nullptr);
				return false;
			}

			vkDestroyFence(_device, fence, nullptr);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::SwapBuffer()
		{
			if (_swapChain == nullptr) // For exemple if the window is hidden the size will be 0 and not swap chain are created
				return false;

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 0;
			presentInfo.pWaitSemaphores = nullptr;

			VkSwapchainKHR swapChains[] = { _swapChain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &_currentImageIndex;
			presentInfo.pResults = nullptr;

			VkResult result = vkQueuePresentKHR(_presentQueue, &presentInfo);
			if (result != VK_SUCCESS)
			{
				OUTPUT_ERROR("Vulkan: Unable to present frame!\n");

				if (result == VK_ERROR_OUT_OF_DATE_KHR)
				{
					OUTPUT_ERROR("Vulkan: VK_ERROR_OUT_OF_DATE_KHR recreating SwapChain...\n");
					ResizeSwapChain();
				}
				else if (result == VK_SUBOPTIMAL_KHR)
				{
					OUTPUT_ERROR("Vulkan: VK_SUBOPTIMAL_KHR recreating SwapChain...\n");
					ResizeSwapChain();
				}

				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material* RendererVulkan::CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			VkMaterial* mat = new VkMaterial();

			if (mat->Build(vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
			{
				delete mat;
				return nullptr;
			}

			return mat;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* RendererVulkan::CreateMaterialInstance(const Material* material)
		{
			return new VkMaterialInstance(*material);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		CommandBuffer* RendererVulkan::CreateCommandBuffer()
		{
			return new CommandBufferVk();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Buffer* RendererVulkan::CreateBuffer(Buffer::Usage usage)
		{
			return new BufferVk(usage);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererVulkan::SetupImGui()
		{
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = _instance;
			init_info.PhysicalDevice = _selectedGpu->physicalDevice;
			init_info.Device = _device;
			init_info.QueueFamily = _selectedGpu->graphicsAndPresentQueueFamilyIndex;
			init_info.Queue = _graphicsQueue;
			init_info.PipelineCache = VK_NULL_HANDLE;
			init_info.DescriptorPool = _descriptorPool;
			init_info.Allocator = nullptr;
			init_info.MinImageCount = 2;
			init_info.ImageCount = 2;
			init_info.CheckVkResultFn = nullptr;
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			ImGui_ImplVulkan_Init(&init_info, _renderPass);

			// Load Fonts
			// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
			// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
			// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
			// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
			// - Read 'docs/FONTS.txt' for more instructions and details.
			// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
			//io.Fonts->AddFontDefault();
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
			//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
			//IM_ASSERT(font != NULL);

			// Upload Fonts
			{
				// Use any command queue
				//VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
				VkCommandBuffer command_buffer = VK_NULL_HANDLE;

				BeginSingleTimeCommands(&command_buffer);

				ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

				EndSingleTimeCommands(command_buffer);

				ImGui_ImplVulkan_DestroyFontUploadObjects();
			}

			return true;
		}
	}
}
