#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitVisibleIf.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param isVisible
	ReflectionTraitVisibleIf::ReflectionTraitVisibleIf(Predicate isVisible)
	: ReflectionTrait()
	, _isVisible(std::move(isVisible))
	{
	}

	/// @brief
	/// @param owner
	/// @return
	bool ReflectionTraitVisibleIf::IsVisible(const void* owner) const
	{
		return _isVisible(owner);
	}
}
