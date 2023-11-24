#include "Converter.h"

namespace hod
{
	bool Converter::NextTokensAre(const std::vector<Token>& tokens, uint32_t& index, const std::vector<Token>& expectedTokens, std::vector<std::string>* identifiers)
	{
		if (identifiers != nullptr)
		{
			identifiers->clear();
		}
		
		uint32_t currentTokenIndex = index;
		uint32_t expectedTokenIndex = 0;

		uint32_t expectedTokenCount = expectedTokens.size();
		while (expectedTokenIndex < expectedTokenCount)
		{
			const Token& expectedToken = expectedTokens[expectedTokenIndex];

			while (tokens[currentTokenIndex]._type == Token::Space || (expectedTokenIndex > 0 && tokens[currentTokenIndex]._type == Token::NewLine))
			{
				++currentTokenIndex;
			}
			const Token& currentToken = tokens[currentTokenIndex];

			if (currentToken._type != expectedToken._type)
			{
				return false;
			}

			if (expectedToken._type == Token::Identifier && std::get<std::string>(expectedToken._data) != "" && std::get<std::string>(expectedToken._data) != std::get<std::string>(currentToken._data))
			{
				return false;
			}

			if (identifiers != nullptr && currentToken._type == Token::Identifier)
			{
				identifiers->push_back(std::get<std::string>(currentToken._data));
			}

			++currentTokenIndex;
			++expectedTokenIndex;
		}

		index = currentTokenIndex - 1;
		return true;
	}
}
