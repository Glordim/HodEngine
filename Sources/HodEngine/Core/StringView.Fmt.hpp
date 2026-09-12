#pragma once
#include "HodEngine/Core/StringView.hpp"

#include <fmt/format.h>

// Optional fmt::format() support, split out of StringView.hpp: fmt/format.h itself drags in <string> and
// <string_view>, so only pay for it in translation units that actually format a StringView.
template<>
struct fmt::formatter<hod::StringView, char> : fmt::formatter<std::string_view, char>
{
	constexpr formatter() noexcept = default;

	template<class ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return fmt::formatter<std::string_view, char>::parse(ctx);
	}

	template<class FmtContext>
	auto format(const hod::StringView& stringView, FmtContext& ctx) const
	{
		return fmt::formatter<std::string_view, char>::format(std::string_view(stringView.Data(), stringView.Size()), ctx);
	}
};
