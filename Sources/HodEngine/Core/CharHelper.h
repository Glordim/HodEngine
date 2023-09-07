#pragma once

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
}
