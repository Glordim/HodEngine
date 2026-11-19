#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/String.hpp"

#include <functional>
#include <string_view>

namespace hod::inline core
{
	/// @brief A property the editor shows disabled when its owner doesn't use it (its value ignored in that state: another property's value
	/// makes it pointless), the reason in its tooltip, the property deciding (its name) highlighted while it is hovered:
	/// HOD_PROPERTY(_elasticity)->AddTrait<ReflectionTraitEnabledIf>(&ScrollView::IsElastic, "Unused: the movement is Clamped", "_movementType");
	/// Several owners edited together: enabled if one of them uses it. See ReflectionTraitVisibleIf to hide it instead
	class HOD_CORE_API ReflectionTraitEnabledIf : public ReflectionTrait
	{
		RTTI(ReflectionTraitEnabledIf, ReflectionTrait)

	public:
		using Predicate = std::function<bool(const void* owner)>;

	public:
		/// @param isEnabled A method of the owner (the object having the property)
		/// @param reason Why it is disabled (its tooltip), none by default
		/// @param decidingProperty The owner's property deciding it (its name, as written in its class), highlighted while it is hovered
		template<typename _Owner_>
		ReflectionTraitEnabledIf(bool (_Owner_::*isEnabled)() const, std::string_view reason = {}, std::string_view decidingProperty = {});
		ReflectionTraitEnabledIf(Predicate isEnabled, std::string_view reason = {}, std::string_view decidingProperty = {});

	public:
		/// @param owner The object having the property
		bool          IsEnabled(const void* owner) const;
		const String& GetReason() const;
		const String& GetDecidingProperty() const;

	private:
		Predicate _isEnabled;
		String    _reason;
		String    _decidingProperty;
	};

	/// @brief
	/// @tparam _Owner_
	/// @param isEnabled
	/// @param reason
	/// @param decidingProperty
	template<typename _Owner_>
	ReflectionTraitEnabledIf::ReflectionTraitEnabledIf(bool (_Owner_::*isEnabled)() const, std::string_view reason, std::string_view decidingProperty)
	: ReflectionTraitEnabledIf([isEnabled](const void* owner) { return (static_cast<const _Owner_*>(owner)->*isEnabled)(); }, reason, decidingProperty)
	{
	}
}
