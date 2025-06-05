#pragma once

#include "HodEngine/Core/String.hpp"
#include <string_view>

namespace hod
{
	constexpr bool IsNumeric(char c)
	{
		return (c >= '0' && c <= '9');
	}

	constexpr bool IsAlphabetic(char c)
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	constexpr bool IsAlphanumeric(char c)
	{
		return (IsAlphabetic(c) || IsNumeric(c));
	}

	constexpr bool IsWhitespace(char c)
	{
		return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
	}

	constexpr bool IsUpper(char c)
	{
		return (c >= 'A' && c <= 'Z');
	}

	constexpr bool IsLower(char c)
	{
		return (c >= 'a' && c <= 'z');
	}

	constexpr char ToUpper(char c)
	{
		return c + ('A' - 'a');
	}

	constexpr char ToLower(char c)
	{
		return c + ('a' - 'A');
	}

	String GenerateDisplayName(const std::string_view& name);
}
