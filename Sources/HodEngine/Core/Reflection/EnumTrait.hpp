#pragma once

#include <string_view>
#include <type_traits>

namespace hod
{
	class EnumTrait
	{
	public:
		template<typename Enum_, typename Underlying_ = std::underlying_type_t<std::decay_t<Enum_>>>
		[[nodiscard]] static constexpr uint32_t GetCount() noexcept;

		template<typename Enum_, typename Underlying_ = std::underlying_type_t<std::decay_t<Enum_>>>
		[[nodiscard]] static constexpr decltype(auto) GetValues() noexcept;

		template<auto Enum_>
		[[nodiscard]] static constexpr std::string_view ToString() noexcept;

		template<typename Enum_, typename Underlying_ = std::underlying_type_t<std::decay_t<Enum_>>>
		[[nodiscard]] static constexpr std::string_view ToString(Enum_ value) noexcept;

	private:
		template<typename Enum_, int Offset_, int... Index_>
		[[nodiscard]] static constexpr uint32_t GetCountInternal(std::integer_sequence<int, Index_...>) noexcept;

		template<typename Enum_, int Offset_, int... Index_>
		[[nodiscard]] static constexpr decltype(auto) GetValuesInternal(std::integer_sequence<int, Index_...>) noexcept;

		template<typename Enum_, Enum_ Value_>
		[[nodiscard]] static constexpr std::string_view ToStringInternalStatic() noexcept;

		template<typename Enum_, int Offset_, int... Index_>
		[[nodiscard]] static constexpr std::string_view ToStringInternal(Enum_ value, std::integer_sequence<int, Index_...>) noexcept;
	};
}

#include "HodEngine/Core/Reflection/EnumTrait.inl"
