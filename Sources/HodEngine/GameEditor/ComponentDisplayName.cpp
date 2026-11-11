#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentDisplayName.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <string_view>

namespace hod::inline editor
{
	/// @brief
	/// @param descriptor
	/// @return
	String GetComponentDisplayName(const ReflectionDescriptor& descriptor)
	{
		const String&              displayName = descriptor.GetDisplayName();
		constexpr std::string_view suffix = " Component";
		if (displayName.Size() > suffix.size() && std::string_view(displayName.CStr() + displayName.Size() - suffix.size(), suffix.size()) == suffix)
		{
			return String(std::string_view(displayName.CStr(), displayName.Size() - suffix.size()));
		}
		return displayName;
	}
}
