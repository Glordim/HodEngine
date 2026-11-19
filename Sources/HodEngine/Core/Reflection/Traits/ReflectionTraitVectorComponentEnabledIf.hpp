#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/String.hpp"

#include <cstdint>
#include <functional>
#include <string_view>
#include <type_traits>

namespace hod::inline core
{
	/// @brief The components of a vector property (Vector2, Vector4) the editor shows disabled when its owner doesn't use them (one axis of
	/// a size unused in a mode...), each by its own method of the owner (nullptr: always enabled), with the reason in its tooltip and the
	/// owner's property deciding it (its name) highlighted while it is hovered:
	/// HOD_PROPERTY(_size)->AddTrait<ReflectionTraitVectorComponentEnabledIf>(&LayoutParams::IsSizeUsedX, "Unused: sized from the content", "_widthMode",
	///                                                                         &LayoutParams::IsSizeUsedY, "Unused: sized from the content", "_heightMode");
	/// Several owners edited together: a component is enabled if one of them uses it. ReflectionTraitEnabledIf for the whole property
	class HOD_CORE_API ReflectionTraitVectorComponentEnabledIf : public ReflectionTrait
	{
		RTTI(ReflectionTraitVectorComponentEnabledIf, ReflectionTrait)

	public:
		static constexpr uint32_t MaxComponentCount = 4;

		using Predicate = std::function<bool(const void* owner)>;

	public:
		template<typename _Owner_>
		using Method = bool (_Owner_::*)() const;

		/// @param x, y, z, w Methods of the owner (the object having the property, given by x), nullptr: the component is always enabled
		/// @param reasonX, reasonY, reasonZ, reasonW Why the component is disabled (its tooltip), none if empty
		/// @param decidingX, decidingY, decidingZ, decidingW The owner's property deciding it (its name, as written in its class), none if empty
		template<typename _Owner_>
		ReflectionTraitVectorComponentEnabledIf(Method<_Owner_> x, std::string_view reasonX, std::string_view decidingX, std::type_identity_t<Method<_Owner_>> y,
		                                        std::string_view reasonY, std::string_view decidingY, std::type_identity_t<Method<_Owner_>> z = nullptr,
		                                        std::string_view reasonZ = {}, std::string_view decidingZ = {}, std::type_identity_t<Method<_Owner_>> w = nullptr,
		                                        std::string_view reasonW = {}, std::string_view decidingW = {});

	public:
		/// @param component 0: X
		/// @param owner The object having the property
		bool IsEnabled(uint32_t component, const void* owner) const;
		/// @return Why the component is disabled, nullptr without a reason
		const char* GetReason(uint32_t component) const;
		/// @return The owner's property deciding whether the component is enabled, nullptr without one
		const char* GetDecidingProperty(uint32_t component) const;

	private:
		template<typename _Owner_>
		static Predicate MakePredicate(bool (_Owner_::*isEnabled)() const);

	private:
		Predicate _predicates[MaxComponentCount];
		String    _reasons[MaxComponentCount];
		String    _decidingProperties[MaxComponentCount];
	};

	/// @brief
	/// @tparam _Owner_
	/// @param x
	/// @param reasonX
	/// @param decidingX
	/// @param y
	/// @param reasonY
	/// @param decidingY
	/// @param z
	/// @param reasonZ
	/// @param decidingZ
	/// @param w
	/// @param reasonW
	/// @param decidingW
	template<typename _Owner_>
	ReflectionTraitVectorComponentEnabledIf::ReflectionTraitVectorComponentEnabledIf(Method<_Owner_> x, std::string_view reasonX, std::string_view decidingX,
	                                                                                 std::type_identity_t<Method<_Owner_>> y, std::string_view reasonY,
	                                                                                 std::string_view decidingY, std::type_identity_t<Method<_Owner_>> z,
	                                                                                 std::string_view reasonZ, std::string_view decidingZ,
	                                                                                 std::type_identity_t<Method<_Owner_>> w, std::string_view reasonW,
	                                                                                 std::string_view decidingW)
	: ReflectionTrait()
	, _predicates {MakePredicate(x), MakePredicate(y), MakePredicate(z), MakePredicate(w)}
	, _reasons {String(reasonX), String(reasonY), String(reasonZ), String(reasonW)}
	, _decidingProperties {String(decidingX), String(decidingY), String(decidingZ), String(decidingW)}
	{
	}

	/// @brief
	/// @tparam _Owner_
	/// @param isEnabled
	/// @return
	template<typename _Owner_>
	ReflectionTraitVectorComponentEnabledIf::Predicate ReflectionTraitVectorComponentEnabledIf::MakePredicate(bool (_Owner_::*isEnabled)() const)
	{
		if (isEnabled == nullptr)
		{
			return nullptr;
		}
		return [isEnabled](const void* owner) { return (static_cast<const _Owner_*>(owner)->*isEnabled)(); };
	}
}
