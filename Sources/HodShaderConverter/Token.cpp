#include "Token.h"

namespace hod
{
    std::array<std::string_view, Token::Type::Count> Token::_labels = {
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
		"\n",		// NewLine
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
}
