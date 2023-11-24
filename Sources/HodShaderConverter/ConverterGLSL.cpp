#include "ConverterGLSL.h"

#include <map>
#include <string>
#include <string_view>

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
		{ "Texture2D", "texture2D " },
		{ "Texture3D", "sampler3D" },
		{ "Texture1DArray", "sampler1DArray" },
		{ "Texture2DArray", "sampler2DArray" },
		{ "TextureCube", "samplerCube" },
		{ "SamplerState", "sampler" },
	};

	/// @brief 
	/// @param tokens 
	/// @return 
	bool ConverterGLSL::Convert(const std::vector<Token>& inTokens, std::vector<Token>& outTokens)
	{
		outTokens.emplace_back(Token::Type::Identifier, "#version");
		outTokens.emplace_back(Token::Type::IntegerValue, 450);
		outTokens.emplace_back(Token::Type::Identifier, "core");
		outTokens.emplace_back(Token::Type::NewLine, 0);
		outTokens.emplace_back(Token::Type::NewLine, 0);

		bool inMainFuction = false;
		bool inInStruct = false;
		bool inOutStruct = false;
		int locationIndex = 0;
		bool nextMustBeAnType = false;

		std::vector<std::string> identifiers;

		std::map<std::string, std::string> inVariableToQualifier;
		std::map<std::string, std::string> outVariableToQualifier;

		for (uint32_t index = 0; index < inTokens.size(); ++index)
		{
			const Token& token = inTokens[index];

			if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "IN"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inInStruct = true;
				locationIndex = 0;
				nextMustBeAnType = true;
				continue;
			}
			else if (inInStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				inInStruct = false;
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}
			else if (inInStruct == true && NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::Colon, 0}, {Token::Type::Identifier, ""}, {Token::Type::Semicolon, 0}}, &identifiers))
			{
				if (identifiers[2] != "SV_POSITION"/* && identifiers[2] != "SV_TARGET"*/)
				{
					outTokens.emplace_back(Token::Type::Identifier, "layout");
					outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
					outTokens.emplace_back(Token::Type::Identifier, "location");
					outTokens.emplace_back(Token::Type::Assign, 0);
					outTokens.emplace_back(Token::Type::IntegerValue, locationIndex);
					outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
					outTokens.emplace_back(Token::Type::Identifier, "in");
					
					auto it = _identifierMap.find(identifiers[0]);
					if (it != _identifierMap.end())
					{
						outTokens.emplace_back(Token::Type::Identifier, std::string(it->second));
					}
					else
					{
						return false;
					}

					outTokens.emplace_back(Token::Type::Identifier, "in_" + identifiers[1]);
					outTokens.emplace_back(Token::Type::Semicolon, 0);

					++locationIndex;
				}
				
				inVariableToQualifier.emplace(identifiers[1], identifiers[2]);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "OUT"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inOutStruct = true;
				locationIndex = 0;
				nextMustBeAnType = true;
				continue;
			}
			else if (inOutStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				inOutStruct = false;
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}
			else if (inOutStruct == true && NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::Colon, 0}, {Token::Type::Identifier, ""}, {Token::Type::Semicolon, 0}}, &identifiers))
			{
				if (identifiers[2] != "SV_POSITION"/* && identifiers[2] != "SV_TARGET"*/)
				{
					outTokens.emplace_back(Token::Type::Identifier, "layout");
					outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
					outTokens.emplace_back(Token::Type::Identifier, "location");
					outTokens.emplace_back(Token::Type::Assign, 0);
					outTokens.emplace_back(Token::Type::IntegerValue, locationIndex);
					outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
					outTokens.emplace_back(Token::Type::Identifier, "out");
					
					auto it = _identifierMap.find(identifiers[0]);
					if (it != _identifierMap.end())
					{
						outTokens.emplace_back(Token::Type::Identifier, std::string(it->second));
					}
					else
					{
						return false;
					}

					outTokens.emplace_back(Token::Type::Identifier, "out_" + identifiers[1]);
					outTokens.emplace_back(Token::Type::Semicolon, 0);

					++locationIndex;
				}

				outVariableToQualifier.emplace(identifiers[1], identifiers[2]);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::Colon, 0}, {Token::Type::Identifier, "register"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::Identifier, ""}, {Token::Type::ClosingParenthesis, 0}, {Token::Type::Semicolon, 0}}, &identifiers))
			{
				int binding = 0;
				if (identifiers[3][0] == 't') // texture
				{
					binding = 0;
				}
				else if (identifiers[3][0] == 's') // sampler
				{
					binding = 1;
				}

				int set = std::atoi(identifiers[3].c_str() + 1);

				outTokens.emplace_back(Token::Type::Identifier, "layout");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, "set");
				outTokens.emplace_back(Token::Type::Assign, 0);
				outTokens.emplace_back(Token::Type::IntegerValue, set);
				outTokens.emplace_back(Token::Type::Comma, 0);
				outTokens.emplace_back(Token::Type::Identifier, "binding");
				outTokens.emplace_back(Token::Type::Assign, 0);
				outTokens.emplace_back(Token::Type::IntegerValue, binding);
				outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, "uniform");

				auto it = _identifierMap.find(identifiers[0]);
				if (it != _identifierMap.end())
				{
					outTokens.emplace_back(Token::Type::Identifier, std::string(it->second));
				}
				else
				{
					return false;
				}

				outTokens.emplace_back(Token::Type::Identifier, identifiers[1]);
				outTokens.emplace_back(Token::Type::Semicolon, 0);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Dot, 0}, {Token::Type::Identifier, "Sample"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::Identifier, ""}}, &identifiers))
			{
				outTokens.emplace_back(Token::Type::Identifier, "texture");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, "sampler2D");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, identifiers[0]);
				outTokens.emplace_back(Token::Type::Comma, 0);
				outTokens.emplace_back(Token::Type::Identifier, identifiers[2]);
				outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, "void"}, {Token::Type::Identifier, "main"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::ClosingParenthesis, 0}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				outTokens.emplace_back(Token::Type::Identifier, "void");
				outTokens.emplace_back(Token::Type::Identifier, "main");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);

				inMainFuction = true;
				continue;
			}
			else if (inMainFuction == true && token._type == Token::Type::ClosingCurlyBracket)
			{
				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				inMainFuction = false;
				continue;
			}
			else if (inMainFuction == true && NextTokensAre(inTokens, index, {{Token::Type::Identifier, "IN"}, {Token::Type::Dot, 0}, {Token::Type::Identifier, ""}}, &identifiers))
			{
				auto it = inVariableToQualifier.find(identifiers[1]);
				if (it != inVariableToQualifier.end())
				{
					if (it->second == "SV_POSITION")
					{
						outTokens.emplace_back(Token::Type::Identifier, "gl_Position");
					}
					else
					{
						outTokens.emplace_back(Token::Type::Identifier, "in_" + it->first);
					}
				}
				else
				{
					return false;
				}
				continue;
			}
			else if (inMainFuction == true && NextTokensAre(inTokens, index, {{Token::Type::Identifier, "OUT"}, {Token::Type::Dot, 0}, {Token::Type::Identifier, ""}}, &identifiers))
			{
				auto it = outVariableToQualifier.find(identifiers[1]);
				if (it != outVariableToQualifier.end())
				{
					if (it->second == "SV_POSITION")
					{
						outTokens.emplace_back(Token::Type::Identifier, "gl_Position");
					}
					else
					{
						outTokens.emplace_back(Token::Type::Identifier, "out_" + it->first);
					}
				}
				else
				{
					return false;
				}
				continue;
			}

			switch (token._type)
			{
				case Token::Type::CBuffer:
				{
					outTokens.emplace_back(Token::Type::Identifier, "layout");
					outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
					outTokens.emplace_back(Token::Type::Identifier, "push_constant");
					outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
					outTokens.emplace_back(Token::Type::Identifier, "uniform");
				}
				break;

				case Token::Type::Identifier:
				{
					std::string value = std::get<std::string>(token._data);
					auto it = _identifierMap.find(value);
					if (it != _identifierMap.end())
					{
						outTokens.emplace_back(Token::Type::Identifier, std::string(it->second));
					}
					else
					{
						outTokens.emplace_back(Token::Type::Identifier, value);
					}
				}
				break;

				default:
				{
					outTokens.push_back(token);
				}
				break;
			}
		}

		return true;
	}
}
