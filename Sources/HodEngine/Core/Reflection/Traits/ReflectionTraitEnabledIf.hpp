#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include <functional>
#include <string_view>

namespace hod::inline core
{
	/// @brief A property the editor shows disabled when its owner doesn't use it (its value ignored in that state: another property's value
	/// makes it pointless), the reason in its tooltip, the properties deciding (their names) highlighted while it is hovered:
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
		/// @param decidingProperties The owner's properties deciding it (their names, as written in its class), highlighted while it is hovered
		template<typename _Owner_, typename... _Names_>
		ReflectionTraitEnabledIf(bool (_Owner_::*isEnabled)() const, std::string_view reason = {}, const _Names_&... decidingProperties);
		template<typename... _Names_>
		ReflectionTraitEnabledIf(Predicate isEnabled, std::string_view reason = {}, const _Names_&... decidingProperties);

	public:
		/// @param owner The object having the property
		bool                  IsEnabled(const void* owner) const;
		const String&         GetReason() const;
		const Vector<String>& GetDecidingProperties() const;

	private:
		void Init(Predicate isEnabled, std::string_view reason);

	private:
		Predicate      _isEnabled;
		String         _reason;
		Vector<String> _decidingProperties;
	};

	/// @brief
	/// @tparam _Owner_
	/// @param isEnabled
	/// @param reason
	/// @param decidingProperties
	template<typename _Owner_, typename... _Names_>
	ReflectionTraitEnabledIf::ReflectionTraitEnabledIf(bool (_Owner_::*isEnabled)() const, std::string_view reason, const _Names_&... decidingProperties)
	: ReflectionTraitEnabledIf(Predicate([isEnabled](const void* owner) { return (static_cast<const _Owner_*>(owner)->*isEnabled)(); }), reason, decidingProperties...)
	{
	}

	/// @brief
	/// @tparam ..._Names_
	/// @param isEnabled
	/// @param reason
	/// @param decidingProperties
	template<typename... _Names_>
	ReflectionTraitEnabledIf::ReflectionTraitEnabledIf(Predicate isEnabled, std::string_view reason, const _Names_&... decidingProperties)
	: ReflectionTrait()
	{
		Init(std::move(isEnabled), reason);
		(_decidingProperties.PushBack(String(std::string_view(decidingProperties))), ...);
	}
}
