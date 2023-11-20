#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <array>
#include <string_view>

namespace hod
{
	class Stream;

	struct ShaderLangToken
	{
		enum Type
		{
			Arrow = 0,
			Assign,
			BoolFalse,
			BoolTrue,
			Break,
			CBuffer,
			ClosingParenthesis,
			ClosingCurlyBracket,
			ClosingSquareBracket,
			Colon,
			Comma,
			Const,
			Continue,
			Discard,
			Divide,
			DivideAssign,
			Dot,
			Equal,
			Else,
			EndOfStream,
			FloatingPointValue,
			For,
			GreaterThan,
			GreaterThanEqual,
			IntegerValue,
			Identifier,
			If,
			LessThan,
			LessThanEqual,
			LogicalAnd,
			LogicalAndAssign,
			LogicalOr,
			LogicalOrAssign,
			Multiply,
			MultiplyAssign,
			Minus,
			MinusAssign,
			Modulo,
			ModuloAssign,
			Not,
			NotEqual,
			Plus,
			PlusAssign,
			OpenCurlyBracket,
			OpenSquareBracket,
			OpenParenthesis,
			Return,
			Semicolon,
			Space,
			StringValue,
			Struct,
			While,

			Count
		};

		static std::array<std::string_view, Type::Count> _tokenLabel;

		Type                                        _type;
		std::variant<int, float, std::string> 		_data;
	};

	class Lexer
	{
	public:
		Lexer() = default;
		Lexer(const Lexer&) = delete;
		Lexer(Lexer&&) = delete;
		~Lexer() = default;

		Lexer& operator=(const Lexer&) = delete;
		Lexer& operator=(Lexer&&) = delete;

	public:
		bool Tokenize(Stream& stream, std::vector<ShaderLangToken>& tokens);
	};
}
