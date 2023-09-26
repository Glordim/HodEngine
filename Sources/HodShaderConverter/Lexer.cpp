#include "Lexer.h"

#include <HodEngine/Core/CharHelper.h>
#include <HodEngine/Core/Output.h>
#include <HodEngine/Core/Stream/Stream.h>

#include <array>
#include <charconv>

namespace hod
{
	bool Lexer::Tokenize(Stream& stream, std::vector<ShaderLangToken>& tokens)
	{
		if (stream.CanRead() == false)
		{
			OUTPUT_ERROR("Lexer: Unable to read stream");
			return false;
		}

		while (true)
		{
			char c = stream.Peek();
			if (IsWhitespace(c) == true)
			{
				stream.Ignore();
			}
			else if (IsAlphabetic(c) == true)
			{
				uint32_t              readedBytes;
				std::array<char, 255> tokenBuffer;

				if (stream.ReadUntil(tokenBuffer.data(), tokenBuffer.max_size(), readedBytes, [](char c) { return IsAlphanumeric(c) || c == '_' || c == '-'; }) == false)
				{
					OUTPUT_ERROR("Lexer: Invalid token (too long)");
					return false;
				}
				std::string_view tokenName(tokenBuffer.data(), readedBytes);
				if (tokenName == "struct")
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::Struct;
					tokens.push_back(token);
				}
				else if (tokenName == "cbuffer")
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::CBuffer;
					tokens.push_back(token);
				}
				else if (tokenName == "return")
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::Return;
					tokens.push_back(token);
				}
				else
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::Identifier;
					token._data = std::string(tokenName);
					tokens.push_back(token);
				}
			}
			else if (IsNumeric(c) == true)
			{
				uint32_t              readedBytes;
				std::array<char, 255> tokenBuffer;

				if (stream.ReadUntil(tokenBuffer.data(), tokenBuffer.max_size(), readedBytes, &IsNumeric) == false)
				{
					OUTPUT_ERROR("Lexer: Invalid token (too long)");
					return false;
				}
				ShaderLangToken token;
				token._type = ShaderLangToken::IntegerValue;
				int value = 0;
				std::from_chars(tokenBuffer.data(), tokenBuffer.data() + readedBytes, value);
				token._data = value;
				tokens.push_back(token);
			}
			else if (c == '{')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::OpenCurlyBracket;
				tokens.push_back(token);
			}
			else if (c == '}')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::ClosingCurlyBracket;
				tokens.push_back(token);
			}
			else if (c == '(')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::OpenParenthesis;
				tokens.push_back(token);
			}
			else if (c == ')')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::ClosingParenthesis;
				tokens.push_back(token);
			}
			else if (c == ';')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Semicolon;
				tokens.push_back(token);
			}
			else if (c == ':')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Colon;
				tokens.push_back(token);
			}
			else if (c == '.')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Dot;
				tokens.push_back(token);
			}
			else if (c == '*')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Multiply;
				tokens.push_back(token);
			}
			else if (c == '+')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Plus;
				tokens.push_back(token);
			}
			else if (c == '-')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Minus;
				tokens.push_back(token);
			}
			else if (c == '/')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Divide;
				tokens.push_back(token);
			}
			else if (c == ',')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Comma;
				tokens.push_back(token);
			}
			else if (c == '=')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::Equal;
				tokens.push_back(token);
			}
			else if (c == '<')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::LessThan;
				tokens.push_back(token);
			}
			else if (c == '>')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::GreaterThan;
				tokens.push_back(token);
			}
			else if (c == '\0')
			{
				stream.Ignore();

				ShaderLangToken token;
				token._type = ShaderLangToken::EndOfStream;
				tokens.push_back(token);
				return true;
			}
			else
			{
				OUTPUT_ERROR("ShaderLexer: Unknow token '%c'", c);
				return false;
			}
		}
	}
}
