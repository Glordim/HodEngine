#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ExtensionCollector/DeviceExtensionCollector.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <VulkanMemoryAllocator/vk_mem_alloc.h>

namespace hod::renderer
{
	DeviceExtensionCollector::~DeviceExtensionCollector()
	{
		for (void* feature : _features)
		{
			DefaultAllocator::GetInstance().Free(feature);
		}
	}

	/// @brief
	/// @return
	bool DeviceExtensionCollector::CollectAvailableExtension(VkPhysicalDevice physicalDevice)
	{
		_availableExtensions.Clear();

		uint32_t availableExtensionCount = 0;
		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, nullptr) != VK_SUCCESS)
		{
			return false;
		}

		_availableExtensions.Resize(availableExtensionCount);
		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, _availableExtensions.Data()) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool DeviceExtensionCollector::IsAvailable(const char* extension) const
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

	bool DeviceExtensionCollector::AddRequiredExtension(const char* extension)
	{
		if (IsAvailable(extension))
		{
			_enabledExtensions.PushBack(extension); // TODO check duplicate ?
			return true;
		}
		return false;
	}

	bool DeviceExtensionCollector::AddOptionalExtension(const char* extension)
	{
		if (IsAvailable(extension))
		{
			_enabledExtensions.PushBack(extension); // TODO check duplicate ?
			return true;
		}
		return false;
	}

	const Vector<const char*>& DeviceExtensionCollector::GetEnabledExtensions() const
	{
		return _enabledExtensions;
	}

	void* DeviceExtensionCollector::GetFirstNextFeature() const
	{
		if (_features.Empty())
		{
			return nullptr;
		}
		return _features.Front();
	}
}
