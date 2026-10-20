#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param icon
	ReflectionTraitIcon::ReflectionTraitIcon(const std::string_view& icon)
	: ReflectionTrait()
	, _icon(icon)
	{
	}

	/// @brief
	/// @return
	const String& ReflectionTraitIcon::GetValue() const
	{
		return _icon;
	}
}
