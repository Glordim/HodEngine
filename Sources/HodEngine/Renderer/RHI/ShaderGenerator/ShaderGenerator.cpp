#include "HodEngine/Renderer/Pch.hpp"
#include "ShaderGenerator.hpp"
#include "TokenReader.hpp"
#include "TokenWritter.hpp"

#include <sstream>

namespace hod::renderer
{
	/// @brief 
	/// @param byteCode 
	/// @param type 
	/// @param source 
	/// @return 
	bool ShaderGenerator::GenerateByteCode(std::vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source)
	{
		byteCode.clear();

		std::string specificPlatformSource;
		if (GenerateSource(specificPlatformSource, source) == false)
		{
			return false;
		}

		if (CompileSource(byteCode, type, specificPlatformSource) == false)
		{
			return false;
		}

		return true;
	}

	/// @brief 
	/// @param generatedSource 
	/// @param source 
	/// @return 
	bool ShaderGenerator::GenerateSource(std::string& generatedSource, std::string_view source)
	{
		std::istringstream inputStream(std::string(source), std::ios_base::in);

		std::vector<Token> tokens;
		TokenReader tokenReader;
		if (tokenReader.Parse(inputStream, tokens) == false)
		{
			return false;
		}

		std::vector<Token> convertedTokens;
		convertedTokens.reserve(tokens.size());
		if (ConvertTokens(tokens, convertedTokens) == false)
		{
			return false;
		}

		std::ostringstream outputStream;

		TokenWritter tokenWritter;
		if (tokenWritter.PrettyWrite(outputStream, convertedTokens) == false)
		{
			return false;
		}

		generatedSource = outputStream.str();
		return true;
	}

	/// @brief 
	/// @param tokens 
	/// @param index 
	/// @param expectedTokens 
	/// @param identifiers 
	/// @return 
	bool ShaderGenerator::NextTokensAre(const std::vector<Token>& tokens, uint32_t& index, const std::vector<Token>& expectedTokens, std::vector<std::string>* identifiers)
	{
		if (identifiers != nullptr)
		{
			identifiers->clear();
		}
		
		uint32_t currentTokenIndex = index;
		uint32_t expectedTokenIndex = 0;

		size_t expectedTokenCount = expectedTokens.size();
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
