#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkValidationLayer.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/ExtensionCollector/InstanceExtensionCollector.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include "HodEngine/Core/Debug.hpp"

#include <array>

#if defined(RENDERER_ENABLE_VALIDATION_LAYER)
namespace hod::renderer
{
	VKAPI_ATTR VkBool32 VKAPI_CALL VkValidationLayer::DebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
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

	VkValidationLayer::VkValidationLayer()
	{
		std::array<const char*, 1> validationLayers {"VK_LAYER_KHRONOS_validation"};

		uint32_t availableValidationLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, nullptr);

		Vector<VkLayerProperties> availableValidationLayers(availableValidationLayerCount);
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, availableValidationLayers.Data());

		_enableValidationLayers = true;
		for (size_t i = 0; i < validationLayers.size(); ++i)
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

	const Vector<const char*>& VkValidationLayer::GetEnabledLayers() const
	{
		return _validationLayers;
	}

	bool VkValidationLayer::CollectInstanceExtensionRequirements(InstanceExtensionCollector& instanceExtensionCollector) const
	{
		if (instanceExtensionCollector.AddRequiredExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == false)
		{
			return false;
		}

	#if defined(RENDERER_ENABLE_VALIDATION_LAYER_ADDRESS_BINDING)
		if (instanceExtensionCollector.AddRequiredExtension(VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME) == false)
		{
			return false;
		}
	#endif

		VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessenger = instanceExtensionCollector.AddFeature<VkDebugUtilsMessengerCreateInfoEXT>();
		SetupCreateInfo(debugUtilsMessenger);

		return true;
	}

	void VkValidationLayer::SetupCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const
	{
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.flags = 0;

		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
	#if defined(RENDERER_ENABLE_VALIDATION_LAYER_ADDRESS_BINDING)
		                         | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
	#endif
			;
		createInfo.pfnUserCallback = &VkValidationLayer::DebugUtilsCallback;
		createInfo.pUserData = (void*)this;
	}

	bool VkValidationLayer::CreateMessager()
	{
		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessenger;
		SetupCreateInfo(debugUtilsMessenger);
		debugUtilsMessenger.pNext = nullptr;

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXTFunc =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(RendererVulkan::GetInstance()->GetVkInstance(), "vkCreateDebugUtilsMessengerEXT");

		if (vkCreateDebugUtilsMessengerEXTFunc != nullptr &&
		    vkCreateDebugUtilsMessengerEXTFunc(RendererVulkan::GetInstance()->GetVkInstance(), &debugUtilsMessenger, nullptr, &_messenger) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	void VkValidationLayer::DestroyMessager()
	{
		if (_messenger != VK_NULL_HANDLE)
		{
			PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXTFunc =
				(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(RendererVulkan::GetInstance()->GetVkInstance(), "vkDestroyDebugUtilsMessengerEXT");

			if (vkDestroyDebugUtilsMessengerEXTFunc != nullptr)
			{
				vkDestroyDebugUtilsMessengerEXTFunc(RendererVulkan::GetInstance()->GetVkInstance(), _messenger, nullptr);
				_messenger = VK_NULL_HANDLE;
			}
		}
	}
}
#endif
