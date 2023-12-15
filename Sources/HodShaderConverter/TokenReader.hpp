#pragma once

#include <cstdint>
#include <vector>
#include <istream>
#include <functional>

#include "Token.hpp"

namespace hod
{
	class TokenReader
	{
	public:
		TokenReader() = default;
		TokenReader(const TokenReader&) = delete;
		TokenReader(TokenReader&&) = delete;
		~TokenReader() = default;

		TokenReader& operator=(const TokenReader&) = delete;
		TokenReader& operator=(TokenReader&&) = delete;

	public:

		bool Parse(std::istream& stream, std::vector<Token>& tokens);

	private:

		bool ReadUntil(std::istream& stream, char* buffer, uint32_t bufferSize, uint32_t& readedBytes, std::function<bool(char)> untilCondition);
	};
}
