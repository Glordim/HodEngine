#pragma once

#include <string_view>
#include <type_traits>

namespace hod
{
	class EnumTrait
	{
	public:
		template<auto Enum>
			requires(std::is_enum_v<decltype(Enum)>)
		[[nodiscard]] static consteval std::string_view ToString() noexcept;

		template<typename Enum, std::underlying_type_t<Enum> Min, std::underlying_type_t<Enum> Max>
			requires(std::is_enum_v<Enum>)
		[[nodiscard]] static consteval auto GetEnumNames() noexcept;

	private:
		template<typename Enum, std::underlying_type_t<Enum> Min, std::underlying_type_t<Enum> Max, std::size_t... Indices>
			requires(std::is_enum_v<Enum>)
		[[nodiscard]] static consteval auto GetEnumNamesImpl(std::index_sequence<Indices...>) noexcept;
	};
}

#include "HodEngine/Core/Reflection/EnumTrait.inl"
