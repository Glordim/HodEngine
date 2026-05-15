#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ExtensionCollector/InstanceExtensionCollector.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstdint>
#include <vk_mem_alloc.h>

namespace hod::inline renderer
{
	InstanceExtensionCollector::~InstanceExtensionCollector()
	{
		for (void* feature : _features)
		{
			DefaultAllocator::GetInstance().Free(feature);
		}
	}

	/// @brief
	/// @return
	bool InstanceExtensionCollector::CollectAvailableExtension(const char* layer)
	{
		uint32_t availableExtensionCount = 0;
		if (vkEnumerateInstanceExtensionProperties(layer, &availableExtensionCount, nullptr) != VK_SUCCESS)
		{
			OUTPUT_ERROR("Vulkan: Unable to collect available extensions for VkInstance");
			return false;
		}

		uint32_t currentExtensionCount = _availableExtensions.Size();
		_availableExtensions.Resize(_availableExtensions.Size() + availableExtensionCount);
		if (vkEnumerateInstanceExtensionProperties(layer, &availableExtensionCount, _availableExtensions.Data() + currentExtensionCount) != VK_SUCCESS)
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
			if (std::strcmp(availableExtension.extensionName, extension) == 0)
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

	const void* InstanceExtensionCollector::GetFirstNextFeature() const
	{
		if (_features.Empty())
		{
			return nullptr;
		}
		return _features.Front();
	}
}
