#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitEnabledIf.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param isEnabled
	/// @param reason
	void ReflectionTraitEnabledIf::Init(Predicate isEnabled, std::string_view reason)
	{
		_isEnabled = std::move(isEnabled);
		_reason = reason;
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
	const Vector<String>& ReflectionTraitEnabledIf::GetDecidingProperties() const
	{
		return _decidingProperties;
	}
}
