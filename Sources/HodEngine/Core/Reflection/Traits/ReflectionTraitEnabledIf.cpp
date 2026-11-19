#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitEnabledIf.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param isEnabled
	/// @param reason
	ReflectionTraitEnabledIf::ReflectionTraitEnabledIf(Predicate isEnabled, std::string_view reason)
	: ReflectionTrait()
	, _isEnabled(std::move(isEnabled))
	, _reason(reason)
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
}
