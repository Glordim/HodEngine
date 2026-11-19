#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitEnabledIf.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param isEnabled
	/// @param reason
	/// @param decidingProperty
	ReflectionTraitEnabledIf::ReflectionTraitEnabledIf(Predicate isEnabled, std::string_view reason, std::string_view decidingProperty)
	: ReflectionTrait()
	, _isEnabled(std::move(isEnabled))
	, _reason(reason)
	, _decidingProperty(decidingProperty)
	{
	}

	/// @brief
	/// @param owner
	/// @return
	bool ReflectionTraitEnabledIf::IsEnabled(const void* owner) const
	{
		return _isEnabled(owner);
	}

	/// @brief
	/// @return
	const String& ReflectionTraitEnabledIf::GetReason() const
	{
		return _reason;
	}

	/// @brief
	/// @return
	const String& ReflectionTraitEnabledIf::GetDecidingProperty() const
	{
		return _decidingProperty;
	}
}
