#include "Lexer.h"

#include <HodEngine/Core/CharHelper.h>
#include <HodEngine/Core/Output.h>
#include <HodEngine/Core/Stream/Stream.h>

#include <array>
#include <string_view>
#include <charconv>

namespace hod
{
	std::array<std::string_view, ShaderLangToken::Type::Count> ShaderLangToken::_tokenLabel = {
		"->", 		// Arrow
		"=", 		// Assign
		"false", 	// BoolFalse
		"true", 	// BoolTrue
		"break",	// Break
		"cbuffer",	// CBuffer
		")",		// ClosingParenthesis
		"}",		// ClosingCurlyBracket
		"]",		// ClosingSquareBracket
		":",		// Colon
		",",		// Comma
		"const",	// Const
		"continue",	// Continue
		"discard",	// Discard
		"/",		// Divide
		"/=",		// DivideAssign
		".",		// Dot
		"==",		// Equal
		"else",		// Else
		"",			// EndOfStream
		"",			// FloatingPointValue
		"for",		// For
		">",		// GreaterThan
		">=",		// GreaterThanEqual
		"",			// IntegerValue
		"",			// Identifier
		"if",		// If
		"<",		// LessThan
		"<=",		// LessThanEqual
		"&",		// LogicalAnd
		"&=",		// LogicalAndAssign
		"|",		// LogicalOr
		"|=",		// LogicalOrAssign
		"*",		// Multiply
		"*=",		// MultiplyAssign
		"-",		// Minus
		"-=",		// MinusAssign
		"%",		// Modulo
		"%=",		// ModuloAssign
		"!",		// Not
		"!=",		// NotEqual
		"+",		// Plus
		"+=",		// PlusAssign
		"{",		// OpenCurlyBracket
		"[",		// OpenSquareBracket
		"(",		// OpenParenthesis
		"return",	// Return
		";",		// Semicolon
		" ",		// Space
		"",			// StringValue
		"struct",	// Struct
		"while"		// While
	};

	bool Lexer::Tokenize(Stream& stream, std::vector<ShaderLangToken>& tokens)
	{
		if (stream.CanRead() == false)
		{
			OUTPUT_ERROR("Lexer: Unable to read stream");
			return false;
		}

		auto size = stream.GetSize();
		while (stream.Tell() < size)
		{
			auto tell = stream.Tell();
			if (tell >= size)
			{
				return true;
			}

			char c = stream.Peek();
			if (IsWhitespace(c) == true)
			{
				stream.Ignore();

				if (tokens.empty() == false && tokens.back()._type != ShaderLangToken::Space)
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::Space;
					tokens.push_back(token);
				}
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
				if (tokenName == ShaderLangToken::_tokenLabel[ShaderLangToken::Struct])
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::Struct;
					tokens.push_back(token);
				}
				else if (tokenName == ShaderLangToken::_tokenLabel[ShaderLangToken::CBuffer])
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::CBuffer;
					tokens.push_back(token);
				}
				else if (tokenName == ShaderLangToken::_tokenLabel[ShaderLangToken::Return])
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
				token._type = ShaderLangToken::Assign;
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

		return true;
	}
}
