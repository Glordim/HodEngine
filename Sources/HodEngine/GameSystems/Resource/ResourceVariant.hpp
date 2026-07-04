#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>

namespace hod::inline gamesystems
{
	/// @brief Describes the axes a cooked Resource can be declined along (Config, Language), used to pick the right variant at load time.
	struct ResourceVariant
	{
		enum class Config : uint8_t
		{
			Development = (1 << 0),
			Profile = (1 << 1),
			Retail = (1 << 2),

			All = std::numeric_limits<std::underlying_type_t<Config>>::max()
		};

		enum class Language : uint32_t
		{
			ENG = (1 << 0),
			FRE = (1 << 1),

			All = std::numeric_limits<std::underlying_type_t<Language>>::max()
		};

		struct ConfigEntry
		{
			Config           value;
			std::string_view name;
		};

		struct LanguageEntry
		{
			Language         value;
			std::string_view name;
		};

		static constexpr ConfigEntry ConfigEntries[] = {
			{Config::Development, "Development"},
			{Config::Profile, "Profile"},
			{Config::Retail, "Retail"},
		};

		static constexpr LanguageEntry LanguageEntries[] = {
			{Language::ENG, "ENG"},
			{Language::FRE, "FRE"},
		};
	};
}
