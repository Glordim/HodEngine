#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include <functional>

namespace hod::inline core
{
	/// @brief A property the editor hides when its owner doesn't use it (its value ignored in that state: another property's value makes it
	/// pointless): HOD_PROPERTY(_curve)->AddTrait<ReflectionTraitVisibleIf>(&Tween::HasCustomEasing);
	/// Several owners edited together: shown if one of them uses it. See ReflectionTraitEnabledIf to show it disabled instead, with a reason
	class HOD_CORE_API ReflectionTraitVisibleIf : public ReflectionTrait
	{
		RTTI(ReflectionTraitVisibleIf, ReflectionTrait)

	public:
		using Predicate = std::function<bool(const void* owner)>;

	public:
		/// @param isVisible A method of the owner (the object having the property)
		template<typename _Owner_>
		ReflectionTraitVisibleIf(bool (_Owner_::*isVisible)() const);
		ReflectionTraitVisibleIf(Predicate isVisible);

	public:
		/// @param owner The object having the property
		bool IsVisible(const void* owner) const;

	private:
		Predicate _isVisible;
	};

	/// @brief
	/// @tparam _Owner_
	/// @param isVisible
	template<typename _Owner_>
	ReflectionTraitVisibleIf::ReflectionTraitVisibleIf(bool (_Owner_::*isVisible)() const)
	: ReflectionTraitVisibleIf([isVisible](const void* owner) { return (static_cast<const _Owner_*>(owner)->*isVisible)(); })
	{
	}
}
