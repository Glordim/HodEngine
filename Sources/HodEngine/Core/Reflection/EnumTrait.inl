#include <array>
#include <limits>

#undef max
#undef min

namespace hod
{
	template<auto Enum>
		requires std::is_enum_v<decltype(Enum)>
	consteval std::string_view ToString() noexcept
	{
#if defined(__clang__) || defined(__GNUC__)
		constexpr std::string_view pretty = __PRETTY_FUNCTION__;
		constexpr auto             start = pretty.find("Enum = ") + 4;
		constexpr auto             end = pretty.find(']', start);
		return pretty.substr(start, end - start);
#elif defined(_MSC_VER)
		constexpr std::string_view pretty = __FUNCSIG__;
		constexpr auto             start = pretty.find("enum ") + 5;
		constexpr auto             end = pretty.find('>', start);
		return pretty.substr(start, end - start);
#else
	#error "Unsupported compiler"
#endif
	}

	template<typename Enum, std::underlying_type_t<Enum> Min, std::underlying_type_t<Enum> Max>
	[[nodiscard]] static consteval auto GetEnumNames() noexcept
	{
		constexpr std::size_t count = static_cast<std::size_t>(Max - Min + 1);
		return GetEnumNamesImpl<Enum, Min, Max>(std::make_index_sequence<count> {});
	}

	template<typename Enum, std::underlying_type_t<Enum> Min, std::underlying_type_t<Enum> Max, std::size_t... Indices>
	[[nodiscard]] static consteval auto GetEnumNamesImpl(std::index_sequence<Indices...>) noexcept
	{
		return std::array<std::string_view, sizeof...(Indices)> {ToString<static_cast<Enum>(Min + Indices)>()...};
	}
}
