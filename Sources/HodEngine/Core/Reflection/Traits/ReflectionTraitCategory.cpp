#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCategory.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param category
	ReflectionTraitCategory::ReflectionTraitCategory(const std::string_view& category)
	: ReflectionTrait()
	{
		std::string_view trimmed = category;
		while (trimmed.empty() == false && trimmed.front() == '/')
		{
			trimmed.remove_prefix(1);
		}
		while (trimmed.empty() == false && trimmed.back() == '/')
		{
			trimmed.remove_suffix(1);
		}
		_category = String(trimmed);
	}

	/// @brief
	/// @return
	const String& ReflectionTraitCategory::GetValue() const
	{
		return _category;
	}
}
