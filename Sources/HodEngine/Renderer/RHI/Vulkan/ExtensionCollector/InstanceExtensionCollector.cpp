#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ExtensionCollector/InstanceExtensionCollector.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <VulkanMemoryAllocator/vk_mem_alloc.h>

namespace hod::renderer
{
	/// @brief
	/// @return
	bool InstanceExtensionCollector::CollectAvailableExtension()
	{
		_availableExtensions.Clear();

		uint32_t availableExtensionCount = 0;
		if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to collect available extensions for VkInstance");
			return false;
		}

		_availableExtensions.Resize(availableExtensionCount);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, _availableExtensions.Data()) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to collect available extensions for VkInstance");
			return false;
		}

		return true;
	}

	bool InstanceExtensionCollector::IsAvailable(const char* extension) const
	{
		for (const VkExtensionProperties& availableExtension : _availableExtensions)
		{
			if (std::strcmp(availableExtension.extensionName, extension))
			{
				return true;
			}
		}
		return false;
	}

	bool InstanceExtensionCollector::AddRequiredExtension(const char* extension)
	{
		if (IsAvailable(extension))
		{
			_enabledExtensions.PushBack(extension); // TODO check duplicate ?
			return true;
		}
		OUTPUT_ERROR("Vulkan: missing required extension '{}', please update your driver", extension);
		return false;
	}

	bool InstanceExtensionCollector::AddOptionalExtension(const char* extension)
	{
		if (IsAvailable(extension))
		{
			_enabledExtensions.PushBack(extension); // TODO check duplicate ?
			return true;
		}
		return false;
	}

	const Vector<const char*>& InstanceExtensionCollector::GetEnabledExtensions() const
	{
		return _enabledExtensions;
	}
}
