#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDescription.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param description
	ReflectionTraitDescription::ReflectionTraitDescription(const std::string_view& description)
	: ReflectionTrait()
	, _description(description)
	{
	}

	/// @brief
	/// @return
	const String& ReflectionTraitDescription::GetValue() const
	{
		return _description;
	}
}
