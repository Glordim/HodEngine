#include "HodEngine/Core/CharHelper.hpp"

#include <array>
#include <limits>

#undef max
#undef min

// Enum variable must be in range (-ENUM_RANGE / 2, ENUM_RANGE /2) for signed types
// Enum variable must be in range (0, ENUM_RANGE) for unsigned types
// If you need a larger range, redefine the macro ENUM_RANGE.
#if !defined(ENUM_RANGE)
	#define ENUM_RANGE 64
#endif
static_assert(ENUM_RANGE > 0, "ENUM_RANGE must be positive and greater than zero.");
static_assert(ENUM_RANGE < std::numeric_limits<int>::max(), "ENUM_RANGE must be less INT_MAX.");

namespace
{
	// for small values like char we should check for the right range - we should not stress the compiler more than necessary
	template<typename Underlying_>
	struct Limit final
	{
		static constexpr int Range = sizeof(Underlying_) <= 2
		                                 ? static_cast<int>(std::numeric_limits<Underlying_>::max()) - static_cast<int>(std::numeric_limits<Underlying_>::min()) + 1
		                                 : std::numeric_limits<int>::max();
		static constexpr int Size = std::min(Range, ENUM_RANGE);
		static constexpr int Offset = std::is_signed_v<Underlying_> ? (Size + 1) / 2 : 0;
	};
}

namespace hod
{
	/// @brief
	/// @tparam Enum_
	/// @tparam Underlying_
	/// @return
	template<typename Enum_, typename Underlying_>
	constexpr uint32_t EnumTrait::GetCount() noexcept
	{
		static_assert(std::is_enum_v<std::decay_t<Enum_>>, "Requires an enum type.");

		using Indices = std::make_integer_sequence<int, Limit<Underlying_>::Size>;
		return GetCountInternal<Enum_, Limit<Underlying_>::Offset>(Indices {});
	}

	template<typename Enum_, int Offset_, int... Index_>
	constexpr uint32_t EnumTrait::GetCountInternal(std::integer_sequence<int, Index_...>) noexcept
	{
		constexpr size_t N = sizeof...(Index_);
		// here we create an array of bool where each index indicates whether it belongs to a valid enum entry
		constexpr std::array<bool, N> validIndices {{!EnumTrait::ToStringInternalStatic<Enum_, static_cast<Enum_>(Index_ - Offset_)>().empty()...}};
		// here we count the number of valid enum indices
		constexpr int numValid = ((validIndices[Index_] ? 1 : 0) + ...);

		return numValid;
	}

	/// @brief return a std::array<Enum,N> with all enumeration values (sorted by value)
	/// @tparam Enum_
	/// @tparam Underlying_
	/// @return
	template<typename Enum_, typename Underlying_>
	constexpr decltype(auto) EnumTrait::GetValues() noexcept
	{
		static_assert(std::is_enum_v<std::decay_t<Enum_>>, "Requires an enum type.");

		using Indices = std::make_integer_sequence<int, Limit<Underlying_>::Size>;
		return GetValuesInternal<Enum_, Limit<Underlying_>::Offset>(Indices {});
	}

	template<typename Enum_, int Offset_, int... Index_>
	constexpr decltype(auto) EnumTrait::GetValuesInternal(std::integer_sequence<int, Index_...>) noexcept
	{
		constexpr size_t N = sizeof...(Index_);
// here we create an array of bool where each index indicates whether it belongs to a valid enum entry
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#endif
		constexpr std::array<bool, N> validIndices {{!EnumTrait::ToStringInternalStatic<Enum_, static_cast<Enum_>(Index_ - Offset_)>().empty()...}};
#ifdef __clang__
	#pragma clang diagnostic pop
#endif
		// here we count the number of valid enum indices
		constexpr int numValid = ((validIndices[Index_] ? 1 : 0) + ...);
		// with this information we can build an array of only valid enum entries
		std::array<Enum_, numValid> enumArray {};
		size_t                      enumIdx = 0;
		for (size_t i = 0; i < N && enumIdx < numValid; ++i)
		{
			if (validIndices[i])
			{
				enumArray[enumIdx++] = static_cast<Enum_>(i - Offset_);
			}
		}

		return enumArray;
	}

	/// @brief get the name from an enum variable, returns a constexpr std::string_view
	/// @tparam Enum_
	/// @tparam Underlying_
	/// @param value
	/// @return
	template<typename Enum_, typename Underlying_>
	constexpr std::string_view EnumTrait::ToString(Enum_ value) noexcept
	{
		static_assert(std::is_enum_v<std::decay_t<Enum_>>, "Requires enum type.");

		using Indices = std::make_integer_sequence<int, Limit<Underlying_>::Size>;
		if (static_cast<int>(value) >= ENUM_RANGE || static_cast<int>(value) <= -ENUM_RANGE)
		{
			return {}; // Enum variable out of ENUM_RANGE.
		}
		return ToStringInternal<std::decay_t<Enum_>, Limit<Underlying_>::Offset>(value, Indices {});
	}

	/// @brief get the name from an enum variable, prefer this over the to_string() version if possible, since
	/// this version is much lighter on the compile times and is not restricted to the size limitation
	/// @tparam Enum_
	/// @return
	template<auto Enum_>
	constexpr std::string_view EnumTrait::ToString() noexcept
	{
		static_assert(std::is_enum_v<std::decay_t<decltype(Enum_)>>, "Requires an enum type.");

		return ToStringInternalStatic<decltype(Enum_), Enum_>();
	}

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9)
	inline constexpr auto suffix = sizeof("]") - 1;
	#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
	inline constexpr auto suffix = sizeof(">(void) noexcept") - 1;
	#define FUNCTION_SIGNATURE __FUNCSIG__
#else
	#define FUNCTION_SIGNATURE
	#error Unsupported compiler
#endif

	// this is the hack that we're gonna use
	// we use either __PRETTY_FUNCTION__ or __FUNCSIG__ to analyze the function signature
	// if the enum value doesn't exist it will appear as a number
	// otherwise it will have the correct enum name for the value V
	// to check whether the enum is valid it's enough to check simply if the first letter of the enum is a number
	// this works with MSVC, gcc and clang
	// for the string conversion we simply have to extract the correct substring from the function signature
	template<typename Enum_, Enum_ Value_>
	constexpr std::string_view EnumTrait::ToStringInternalStatic() noexcept
	{
		constexpr std::string_view name {FUNCTION_SIGNATURE};
		constexpr std::size_t      prefix = name.find_last_of(", :)-", name.size() - suffix) + 1;

		if constexpr (!IsNumeric(name[prefix]))
		{
			return name.substr(prefix, name.size() - suffix - prefix);
		}
		else
		{
			return {};
		}
	}

	template<typename Enum_, int Offset_, int... Index_>
	constexpr std::string_view EnumTrait::ToStringInternal(Enum_ value, std::integer_sequence<int, Index_...>) noexcept
	{
		// we have to convert the runtime value to a compile time index
		// this method uses an O(1) lookup via function pointers
		using ToStringFunctionDecl = decltype(&ToStringInternalStatic<Enum_, static_cast<Enum_>(0)>);
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#endif
		constexpr std::array<ToStringFunctionDecl, sizeof...(Index_)> ToStringFunctions {{ToStringInternalStatic<Enum_, static_cast<Enum_>(Index_ - Offset_)>...}};
#ifdef __clang__
	#pragma clang diagnostic pop
#endif
		return ToStringFunctions[size_t(Offset_ + static_cast<int>(value))]();
	}
}
