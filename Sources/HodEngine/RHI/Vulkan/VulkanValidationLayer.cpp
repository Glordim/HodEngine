#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/RHI/Vulkan/VulkanValidationLayer.hpp"

#include "HodEngine/RHI/Vulkan/ExtensionCollector/InstanceExtensionCollector.hpp"
#include "HodEngine/RHI/Vulkan/VulkanRhiDevice.hpp"

#include <HodEngine/Core/Debug.hpp>
#include <HodEngine/Core/OS.hpp>

#include "HodEngine/Core/StaticArray.hpp"

#if defined(RHI_ENABLE_VALIDATION_LAYER)
namespace hod::inline rhi
{
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanValidationLayer::DebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
	                                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		(void)messageType;
		(void)pUserData;

		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			OUTPUT_ERROR("Validation Layer: {}", pCallbackData->pMessage);
			Break();
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

	VulkanValidationLayer::VulkanValidationLayer()
	{
#if !defined(PLATFORM_ANDROID)
		OS::SetEnv("VK_LAYER_PATH", (FileSystem::GetExecutablePath().ParentPath() / "ValidationLayers").GetString().CStr());
#endif

		StaticArray<const char*, 1> validationLayers {"VK_LAYER_KHRONOS_validation"};

		uint32_t availableValidationLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, nullptr);

		Vector<VkLayerProperties> availableValidationLayers(availableValidationLayerCount);
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, availableValidationLayers.Data());

		OUTPUT_MESSAGE("Vulkan: {} validation layer(s) available:", availableValidationLayerCount);
		for (uint32_t i = 0; i < availableValidationLayerCount; ++i)
		{
			OUTPUT_MESSAGE("  Layer: {}", availableValidationLayers[i].layerName);
		}

		_enableValidationLayers = true;
		for (size_t i = 0; i < validationLayers.Size(); ++i)
		{
			const char* validationLayerName = validationLayers[i];

			bool founded = false;

			for (size_t j = 0; j < availableValidationLayerCount; ++j)
			{
				if (strcmp(validationLayerName, availableValidationLayers[j].layerName) == 0)
				{
					founded = true;
					_validationLayers.PushBack(validationLayerName);
					break;
				}
			}

			if (founded == false)
			{
				OUTPUT_ERROR("Vulkan: ValidationLayers are not available, try to update 'Vulkan Runtime'");
				OUTPUT_ERROR("Vulkan: ValidationLayers have been disabled");
				_enableValidationLayers = false;
				break;
			}
		}
	}

	const Vector<const char*>& VulkanValidationLayer::GetEnabledLayers() const
	{
		return _validationLayers;
	}

	bool VulkanValidationLayer::CollectInstanceExtensionRequirements(InstanceExtensionCollector& instanceExtensionCollector) const
	{
		if (_enableValidationLayers == false)
		{
			return true;
		}

		if (instanceExtensionCollector.AddRequiredExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == false)
		{
			return false;
		}

	#if defined(RHI_ENABLE_VALIDATION_LAYER_ADDRESS_BINDING)
		if (instanceExtensionCollector.AddRequiredExtension(VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME) == false)
		{
			return false;
		}
	#endif

		VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessenger = instanceExtensionCollector.AddFeature<VkDebugUtilsMessengerCreateInfoEXT>();
		SetupCreateInfo(debugUtilsMessenger);

		return true;
	}

	void VulkanValidationLayer::SetupCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const
	{
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.flags = 0;

		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
	#if defined(RHI_ENABLE_VALIDATION_LAYER_ADDRESS_BINDING)
		                         | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
	#endif
			;
		createInfo.pfnUserCallback = &VulkanValidationLayer::DebugUtilsCallback;
		createInfo.pUserData = (void*)this;
	}

	bool VulkanValidationLayer::CreateMessager()
	{
		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessenger;
		SetupCreateInfo(debugUtilsMessenger);
		debugUtilsMessenger.pNext = nullptr;

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXTFunc =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanRhiDevice::GetInstance()->GetVkInstance(), "vkCreateDebugUtilsMessengerEXT");

		if (vkCreateDebugUtilsMessengerEXTFunc != nullptr &&
		    vkCreateDebugUtilsMessengerEXTFunc(VulkanRhiDevice::GetInstance()->GetVkInstance(), &debugUtilsMessenger, nullptr, &_messenger) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	void VulkanValidationLayer::DestroyMessager()
	{
		if (_messenger != VK_NULL_HANDLE)
		{
			PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXTFunc =
				(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(VulkanRhiDevice::GetInstance()->GetVkInstance(), "vkDestroyDebugUtilsMessengerEXT");

			if (vkDestroyDebugUtilsMessengerEXTFunc != nullptr)
			{
				vkDestroyDebugUtilsMessengerEXTFunc(VulkanRhiDevice::GetInstance()->GetVkInstance(), _messenger, nullptr);
				_messenger = VK_NULL_HANDLE;
			}
		}
	}
}
#endif
