#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>

namespace hod::inline gamesystems
{
	/// @brief Describes the axes a cooked Resource can be declined along (Language), used to pick the right variant at load time.
	struct ResourceVariant
	{
		enum class Language : uint32_t
		{
			ENG = (1 << 0),
			FRE = (1 << 1),

			All = std::numeric_limits<std::underlying_type_t<Language>>::max()
		};

		struct LanguageEntry
		{
			Language         value;
			std::string_view name;
		};

		static constexpr LanguageEntry LanguageEntries[] = {
			{Language::ENG, "ENG"},
			{Language::FRE, "FRE"},
		};
	};
}
