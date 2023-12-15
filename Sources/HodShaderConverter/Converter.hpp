#pragma once

#include <vector>
#include <string>

#include "Token.hpp"

namespace hod
{
    class Converter
    {
    public:
		Converter() = default;
		Converter(const Converter&) = delete;
		Converter(Converter&&) = delete;
		~Converter() = default;

		Converter&    operator=(const Converter&) = delete;
		Converter&    operator=(Converter&&) = delete;

	public:
	
		virtual bool	Convert(const std::vector<Token>& inTokens, std::vector<Token>& outTokens) = 0;

	protected:

		bool			NextTokensAre(const std::vector<Token>& tokens, uint32_t& index, const std::vector<Token>& expectedTokens, std::vector<std::string>* identifiers = nullptr);
    };
}
