#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace hod
{
	class Stream;
}

namespace hod::renderer
{
	struct ShaderLangToken
	{
		enum Type
		{
			Alias,
			Arrow,
			As,
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
			ConstSelect,
			Continue,
			Discard,
			Divide,
			DivideAssign,
			Dot,
			Equal,
			Else,
			EndOfStream,
			External,
			FloatingPointValue,
			For,
			FunctionDeclaration,
			GreaterThan,
			GreaterThanEqual,
			Include,
			IntegerValue,
			Identifier,
			If,
			In,
			LessThan,
			LessThanEqual,
			Let,
			LogicalAnd,
			LogicalAndAssign,
			LogicalOr,
			LogicalOrAssign,
			Multiply,
			MultiplyAssign,
			Minus,
			MinusAssign,
			Module,
			Modulo,
			ModuloAssign,
			Not,
			NotEqual,
			Plus,
			PlusAssign,
			OpenCurlyBracket,
			OpenSquareBracket,
			OpenParenthesis,
			Option,
			Return,
			Semicolon,
			StringValue,
			Struct,
			While,
		};

		Type                                         _type;
		std::variant<double, long long, std::string> _data;
	};

	class ShaderLangLexer
	{
	public:
		ShaderLangLexer() = default;
		ShaderLangLexer(const ShaderLangLexer&) = delete;
		ShaderLangLexer(ShaderLangLexer&&) = delete;
		~ShaderLangLexer() = default;

		const ShaderLangLexer& operator=(const ShaderLangLexer&) = delete;
		const ShaderLangLexer& operator=(ShaderLangLexer&&) = delete;

	public:
		bool Tokenize(Stream& stream, std::vector<ShaderLangToken>& tokens);
	};
}
