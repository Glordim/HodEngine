#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vulkan/vulkan.h>
#if defined(RENDERER_ENABLE_VALIDATION_LAYER)

namespace hod::renderer
{
	class InstanceExtensionCollector;

	class HOD_RENDERER_API VkValidationLayer
	{
	public:
		VkValidationLayer();

		bool CollectInstanceExtensionRequirements(InstanceExtensionCollector& instanceExtensionCollector) const;

		const Vector<const char*>& GetEnabledLayers() const;

		bool CreateMessager();
		void DestroyMessager();

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		                                                         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	private:
		void SetupCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;

	private:
		bool                _enableValidationLayers = false;
		Vector<const char*> _validationLayers;

		VkDebugUtilsMessengerEXT _messenger = VK_NULL_HANDLE;
	};
}

#endif
