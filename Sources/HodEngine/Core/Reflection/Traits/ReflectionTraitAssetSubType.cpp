#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitAssetSubType.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param expectedSubType
	ReflectionTraitAssetSubType::ReflectionTraitAssetSubType(RttiType expectedSubType)
	: _expectedSubType(expectedSubType)
	{
	}

	/// @brief
	/// @return
	RttiType ReflectionTraitAssetSubType::GetExpectedSubType() const
	{
		return _expectedSubType;
	}
}
