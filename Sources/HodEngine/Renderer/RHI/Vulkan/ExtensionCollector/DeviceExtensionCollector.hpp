#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <vulkan/vulkan.h>

namespace hod::renderer
{
	/// @brief
	class HOD_RENDERER_API DeviceExtensionCollector final
	{
	public:
		DeviceExtensionCollector() = default;
		~DeviceExtensionCollector();

		bool CollectAvailableExtension(VkPhysicalDevice physicalDevice);
		bool AddRequiredExtension(const char* extension);
		bool AddOptionalExtension(const char* extension);

		const Vector<const char*>& GetEnabledExtensions() const;

		template<typename FeatureType>
		FeatureType& AddFeature();

		void* GetFirstNextFeature() const;

	private:
		bool IsAvailable(const char* extension) const;

	private:
		Vector<VkExtensionProperties> _availableExtensions;
		Vector<const char*>           _enabledExtensions;
		Vector<void*>                 _features;
		void**                        _lastNextPtr = nullptr;
	};

	template<typename FeatureType>
	FeatureType& DeviceExtensionCollector::AddFeature()
	{
		FeatureType* feature = static_cast<FeatureType*>(DefaultAllocator::GetInstance().Allocate(sizeof(FeatureType)));
		feature->pNext = nullptr;

		if (_lastNextPtr != nullptr)
		{
			*_lastNextPtr = feature;
		}
		_lastNextPtr = &feature->pNext;

		_features.PushBack(feature);
		return *feature;
	}
}
