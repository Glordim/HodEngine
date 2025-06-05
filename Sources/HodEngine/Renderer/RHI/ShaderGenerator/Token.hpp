#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <variant>
#include <array>
#include "HodEngine/Core/String.hpp"
#include <string_view>

namespace hod::renderer
{
    struct HOD_RENDERER_API Token
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
			NewLine,
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

		Type                                                _type;
		std::variant<int, float, std::string> 		        _data;

		static std::array<std::string_view, Type::Count>    _labels;

		Token() = default;
		Token(Type type, std::variant<int, float, std::string> data)
		: _type(type)
		, _data(data)
		{

		}
	};
}
