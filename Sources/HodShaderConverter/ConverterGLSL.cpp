#include "ConverterGLSL.h"
#include "Lexer.h"

namespace hod
{
	const std::map<const std::string_view, const std::string_view> _identifierMap = {
		{ "void", "void" },
		{ "float", "float" },
		{ "float2", "vec2" },
		{ "float3", "vec3" },
		{ "float4", "vec4" },
		{ "float2x2", "mat2" },
		{ "float3x3", "mat3" },
		{ "float4x4", "mat4" },
		{ "double", "double" },
		{ "double2", "dvec2" },
		{ "double3", "dvec3" },
		{ "double4", "dvec4" },
		{ "double2x2", "dmat2" },
		{ "double3x3", "dmat3" },
		{ "double4x4", "dmat4" },
		{ "int", "int" },
		{ "int2", "ivec2" },
		{ "int3", "ivec3" },
		{ "int4", "ivec4" },
		{ "uint", "uint" },
		{ "uint2", "uvec2" },
		{ "uint3", "uvec3" },
		{ "uint4", "uvec4" },
		{ "bool", "bool" },
		{ "bool2", "bvec2" },
		{ "bool3", "bvec3" },
		{ "bool4", "bvec4" },
		{ "Texture1D", "sampler1D" },
		{ "Texture2D", "sampler2D" },
		{ "Texture3D", "sampler3D" },
		{ "Texture1DArray", "sampler1DArray" },
		{ "Texture2DArray", "sampler2DArray" },
		{ "TextureCube", "samplerCube" },
	};

	bool NextTokensAre(const std::vector<ShaderLangToken>& tokens, uint32_t& index, const std::vector<ShaderLangToken>& expectedTokens, std::vector<std::string>* identifiers = nullptr)
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
			const ShaderLangToken& expectedToken = expectedTokens[expectedTokenIndex];

			while (tokens[currentTokenIndex]._type == ShaderLangToken::Space)
			{
				++currentTokenIndex;
			}
			const ShaderLangToken& currentToken = tokens[currentTokenIndex];

			if (currentToken._type != expectedToken._type)
			{
				return false;
			}

			if (expectedToken._type == ShaderLangToken::Identifier && std::get<std::string>(expectedToken._data) != "" && std::get<std::string>(expectedToken._data) != std::get<std::string>(currentToken._data))
			{
				return false;
			}

			if (identifiers != nullptr && currentToken._type == ShaderLangToken::Identifier)
			{
				identifiers->push_back(std::get<std::string>(currentToken._data));
			}

			++currentTokenIndex;
			++expectedTokenIndex;
		}

		index = currentTokenIndex - 1;
		return true;
	}

	/// @brief 
	/// @param tokens 
	/// @return 
	bool ConverterGLSL::Convert(const std::vector<ShaderLangToken>& tokens)
	{
		int curvyBracketDeep = 0;
		bool inMainFuction = false;
		bool inInStruct = false;
		bool inOutStruct = false;
		int locationIndex = 0;
		bool nextMustBeAnType = false;

		std::vector<std::string> identifiers;

		std::map<std::string, std::string> inVariableToQualifier;
		std::map<std::string, std::string> outVariableToQualifier;

		for (uint32_t index = 0; index < tokens.size(); ++index)
		{
			const ShaderLangToken& token = tokens[index];

			if (NextTokensAre(tokens, index, {{ShaderLangToken::Type::Struct, 0}, {ShaderLangToken::Type::Identifier, "IN"}, {ShaderLangToken::Type::OpenCurlyBracket, 0}}))
			{
				inInStruct = true;
				locationIndex = 0;
				nextMustBeAnType = true;
				continue;
			}
			else if (inInStruct == true && NextTokensAre(tokens, index, {{ShaderLangToken::Type::ClosingCurlyBracket, 0}, {ShaderLangToken::Type::Semicolon, 0}}))
			{
				_result += " ";
				inInStruct = false;
				continue;
			}
			else if (inInStruct == true && NextTokensAre(tokens, index, {{ShaderLangToken::Type::Identifier, ""}, {ShaderLangToken::Type::Identifier, ""}, {ShaderLangToken::Type::Colon, 0}, {ShaderLangToken::Type::Identifier, ""}, {ShaderLangToken::Type::Semicolon, 0}}, &identifiers))
			{
				if (identifiers[2] != "SV_POSITION" && identifiers[2] != "SV_TARGET")
				{
					_result += "layout (location = ";
					_result += std::to_string(locationIndex);
					_result += ") in ";
					auto it = _identifierMap.find(identifiers[0]);
					if (it != _identifierMap.end())
					{
						_result += it->second;
					}
					_result += " in_";
					_result += identifiers[1];
					_result += ";";

					++locationIndex;
				}
				
				inVariableToQualifier.emplace(identifiers[1], identifiers[2]);
				continue;
			}

			if (NextTokensAre(tokens, index, {{ShaderLangToken::Type::Struct, 0}, {ShaderLangToken::Type::Identifier, "OUT"}, {ShaderLangToken::Type::OpenCurlyBracket, 0}}))
			{
				inOutStruct = true;
				locationIndex = 0;
				nextMustBeAnType = true;
				continue;
			}
			else if (inOutStruct == true && NextTokensAre(tokens, index, {{ShaderLangToken::Type::ClosingCurlyBracket, 0}, {ShaderLangToken::Type::Semicolon, 0}}))
			{
				_result += " ";
				inOutStruct = false;
				continue;
			}
			else if (inOutStruct == true && NextTokensAre(tokens, index, {{ShaderLangToken::Type::Identifier, ""}, {ShaderLangToken::Type::Identifier, ""}, {ShaderLangToken::Type::Colon, 0}, {ShaderLangToken::Type::Identifier, ""}, {ShaderLangToken::Type::Semicolon, 0}}, &identifiers))
			{
				if (identifiers[2] != "SV_POSITION" && identifiers[2] != "SV_TARGET")
				{
					_result += "layout (location = ";
					_result += std::to_string(locationIndex);
					_result += ") out ";
					auto it = _identifierMap.find(identifiers[0]);
					if (it != _identifierMap.end())
					{
						_result += it->second;
					}
					_result += " out_";
					_result += identifiers[1];
					_result += ";";

					++locationIndex;
				}

				outVariableToQualifier.emplace(identifiers[1], identifiers[2]);
				continue;
			}

			if (NextTokensAre(tokens, index, {{ShaderLangToken::Type::Identifier, "void"}, {ShaderLangToken::Type::Identifier, "main"}, {ShaderLangToken::Type::OpenParenthesis, 0}, {ShaderLangToken::Type::ClosingParenthesis, 0}, {ShaderLangToken::Type::OpenCurlyBracket, 0}}))
			{
				_result += "void main() {";

				inMainFuction = true;
				continue;
			}
			else if (inMainFuction == true && token._type == ShaderLangToken::Type::ClosingCurlyBracket && inMainFuction == true && curvyBracketDeep == 0)
			{
				_result += "}";
				inMainFuction = false;
				continue;
			}
			else if (inMainFuction == true && NextTokensAre(tokens, index, {{ShaderLangToken::Type::Identifier, "IN"}, {ShaderLangToken::Type::Dot, 0}, {ShaderLangToken::Type::Identifier, ""}}, &identifiers))
			{
				auto it = inVariableToQualifier.find(identifiers[1]);
				if (it != inVariableToQualifier.end())
				{
					if (it->second == "SV_POSITION")
					{
						_result += "gl_Position";
					}
					else
					{
						_result += "in_";
						_result += it->first;
					}
				}
				else
				{
					_result += "ERROR"; // todo
				}
				continue;
			}
			else if (inMainFuction == true && NextTokensAre(tokens, index, {{ShaderLangToken::Type::Identifier, "OUT"}, {ShaderLangToken::Type::Dot, 0}, {ShaderLangToken::Type::Identifier, ""}}, &identifiers))
			{
				auto it = outVariableToQualifier.find(identifiers[1]);
				if (it != outVariableToQualifier.end())
				{
					if (it->second == "SV_POSITION")
					{
						_result += "gl_Position";
					}
					else
					{
						_result += "out_";
						_result += it->first;
					}
				}
				else
				{
					_result += "ERROR"; // todo
				}
				continue;
			}

			switch (token._type)
			{
				case ShaderLangToken::Type::CBuffer:
				{
					_result += "layout(push_constant) uniform";
				}
				break;

				case ShaderLangToken::Type::Identifier:
				{
					std::string value = std::get<std::string>(token._data);
					auto it = _identifierMap.find(value);
					if (it != _identifierMap.end())
					{
						_result += it->second;
					}
					else
					{
						_result += value;
					}
				}
				break;

				case ShaderLangToken::Type::IntegerValue:
				{
					int value = std::get<int>(token._data);
					_result += std::to_string(value);
				}
				break;

				case ShaderLangToken::Type::FloatingPointValue:
				{
					float value = std::get<float>(token._data);
					_result += std::to_string(value);
				}
				break;

				case ShaderLangToken::Type::OpenCurlyBracket:
				{
					++curvyBracketDeep;
					_result += "{";
				}
				break;

				case ShaderLangToken::Type::ClosingCurlyBracket:
				{
					--curvyBracketDeep;
					_result += "}";
				}
				break;

				default:
				{
					_result += ShaderLangToken::_tokenLabel[token._type];
				}
				break;
			}
		}

		return true;
	}
}
