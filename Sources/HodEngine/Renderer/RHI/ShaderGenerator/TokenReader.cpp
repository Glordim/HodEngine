#include "HodEngine/Renderer/Pch.hpp"
#include "TokenReader.hpp"

#include <HodEngine/Core/CharHelper.hpp>
#include "HodEngine/Core/Output/OutputService.hpp"
//#include <HodEngine/Core/Stream/Stream.hpp>

#include <array>
#include <string_view>
#include <charconv>
#include <iostream>
#include <format>

namespace hod::renderer
{
	bool TokenReader::Parse(std::istream& stream, Vector<Token>& tokens)
	{
		/* try catch ?
		if (stream. CanRead() == false)
		{
			OUTPUT_ERROR("Lexer: Unable to read stream");
			return false;
		}
		*/

		while (stream.eof() == false)
		{
			int peek = stream.peek();
			char c = (char)peek;
			if (c == '\n' && tokens.empty() == false && tokens.back()._type == Token::Type::Semicolon)
			{
				stream.ignore();
				
				Token token;
				token._type = Token::NewLine;
				tokens.push_back(token);
			}
			else if (IsWhitespace(c) == true)
			{
				stream.ignore();
			}
			else if (IsAlphabetic(c) == true)
			{
				uint32_t              readedBytes;
				std::array<char, 255> tokenBuffer;

				if (ReadUntil(stream, tokenBuffer.data(), (uint32_t)tokenBuffer.max_size(), readedBytes, [](char c) { return IsAlphanumeric(c) || c == '_' || c == '-'; }) == false)
				{
					std::cerr << "Lexer: Invalid token (too long)\n";
					return false;
				}
				std::string_view tokenName(tokenBuffer.data(), readedBytes);
				if (tokenName == Token::_labels[Token::Struct])
				{
					Token token;
					token._type = Token::Struct;
					tokens.push_back(token);
				}
				else if (tokenName == Token::_labels[Token::CBuffer])
				{
					Token token;
					token._type = Token::CBuffer;
					tokens.push_back(token);
				}
				else if (tokenName == Token::_labels[Token::Return])
				{
					Token token;
					token._type = Token::Return;
					tokens.push_back(token);
				}
				else
				{
					Token token;
					token._type = Token::Identifier;
					token._data = std::string(tokenName);
					tokens.push_back(token);
				}
			}
			else if (IsNumeric(c) == true)
			{
				uint32_t              readedBytes;
				std::array<char, 255> tokenBuffer;

				if (ReadUntil(stream, tokenBuffer.data(), (uint32_t)tokenBuffer.max_size(), readedBytes, &IsNumeric) == false)
				{
					std::cerr << "Lexer: Invalid token (too long)\n";
					return false;
				}
				Token token;
				token._type = Token::IntegerValue;
				int value = 0;
				std::from_chars(tokenBuffer.data(), tokenBuffer.data() + readedBytes, value);
				token._data = value;
				tokens.push_back(token);
			}
			else if (c == '{')
			{
				stream.ignore();

				Token token;
				token._type = Token::OpenCurlyBracket;
				tokens.push_back(token);
			}
			else if (c == '}')
			{
				stream.ignore();

				Token token;
				token._type = Token::ClosingCurlyBracket;
				tokens.push_back(token);
			}
			else if (c == '(')
			{
				stream.ignore();

				Token token;
				token._type = Token::OpenParenthesis;
				tokens.push_back(token);
			}
			else if (c == ')')
			{
				stream.ignore();

				Token token;
				token._type = Token::ClosingParenthesis;
				tokens.push_back(token);
			}
			else if (c == ';')
			{
				stream.ignore();

				Token token;
				token._type = Token::Semicolon;
				tokens.push_back(token);
			}
			else if (c == ':')
			{
				stream.ignore();

				Token token;
				token._type = Token::Colon;
				tokens.push_back(token);
			}
			else if (c == '.')
			{
				stream.ignore();

				Token token;
				token._type = Token::Dot;
				tokens.push_back(token);
			}
			else if (c == '*')
			{
				stream.ignore();

				Token token;
				token._type = Token::Multiply;
				tokens.push_back(token);
			}
			else if (c == '+')
			{
				stream.ignore();

				Token token;
				token._type = Token::Plus;
				tokens.push_back(token);
			}
			else if (c == '-')
			{
				stream.ignore();

				Token token;
				token._type = Token::Minus;
				tokens.push_back(token);
			}
			else if (c == '/')
			{
				stream.ignore();

				Token token;
				token._type = Token::Divide;
				tokens.push_back(token);
			}
			else if (c == ',')
			{
				stream.ignore();

				Token token;
				token._type = Token::Comma;
				tokens.push_back(token);
			}
			else if (c == '=')
			{
				stream.ignore();

				Token token;
				token._type = Token::Assign;
				tokens.push_back(token);
			}
			else if (c == '<')
			{
				stream.ignore();

				Token token;
				token._type = Token::LessThan;
				tokens.push_back(token);
			}
			else if (c == '>')
			{
				stream.ignore();

				Token token;
				token._type = Token::GreaterThan;
				tokens.push_back(token);
			}
			else if (c == '|')
			{
				stream.ignore();

				Token token;
				token._type = Token::LogicalOr;
				tokens.push_back(token);
			}
			else if (c == '&')
			{
				stream.ignore();
				
				Token token;
				token._type = Token::LogicalAnd;
				tokens.push_back(token);
			}
			else if (c == '\0' || peek == EOF)
			{
				stream.ignore();

				Token token;
				token._type = Token::EndOfStream;
				tokens.push_back(token);
				return true;
			}
			else
			{
				std::cerr << std::format("ShaderLexer: Unknow token '{}'\n", c);
				return false;
			}
		}

		return true;
	}

	bool TokenReader::ReadUntil(std::istream& stream, char* buffer, uint32_t bufferSize, uint32_t& readedBytes, std::function<bool(char)> untilCondition)
	{
		readedBytes = 0;

		while (readedBytes < (bufferSize - 1))
		{
			char c = stream.peek();
			if (untilCondition(c) == true)
			{
				buffer[readedBytes] = c;
				stream.ignore();
				++readedBytes;
			}
			else
			{
				return true;
			}
		}
		return false;
	}
}
