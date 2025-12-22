#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <vulkan/vulkan.h>

namespace hod::renderer
{
	/// @brief
	class HOD_RENDERER_API InstanceExtensionCollector final
	{
	public:
		InstanceExtensionCollector() = default;

		bool CollectAvailableExtension();
		bool AddRequiredExtension(const char* extension);
		bool AddOptionalExtension(const char* extension);

		const Vector<const char*>& GetEnabledExtensions() const;

	private:
		bool IsAvailable(const char* extension) const;

	private:
		Vector<VkExtensionProperties> _availableExtensions;
		Vector<const char*>           _enabledExtensions;
	};
}
