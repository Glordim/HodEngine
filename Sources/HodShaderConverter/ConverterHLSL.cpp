#include "ConverterHLSL.hpp"

#include <map>
#include <string>
#include <string_view>

namespace hod
{
	const std::map<const std::string_view, const std::string_view> _identifierMap = {
		{ "void", "void" },
		{ "float", "float" },
		{ "float2", "float2" },
		{ "float3", "float3" },
		{ "float4", "float4" },
		{ "float2x2", "float2x2" },
		{ "float3x3", "float3x3" },
		{ "float4x4", "float4x4" },
		{ "double", "double" },
		{ "double2", "double2" },
		{ "double3", "double3" },
		{ "double4", "double4" },
		{ "double2x2", "double2x2" },
		{ "double3x3", "double3x3" },
		{ "double4x4", "double4x4" },
		{ "int", "int" },
		{ "int2", "int2" },
		{ "int3", "int3" },
		{ "int4", "int4" },
		{ "uint", "uint" },
		{ "uint2", "uint2" },
		{ "uint3", "uint3" },
		{ "uint4", "uint4" },
		{ "bool", "bool" },
		{ "bool2", "bool2" },
		{ "bool3", "bool3" },
		{ "bool4", "bool4" },
		{ "Texture1D", "Texture1D" },
		{ "Texture2D", "Texture2D" },
		{ "Texture3D", "Texture3D" },
		{ "Texture1DArray", "Texture1DArray" },
		{ "Texture2DArray", "Texture2DArray" },
		{ "TextureCube", "TextureCube" },
	};

	/// @brief 
	/// @param tokens 
	/// @return 
	bool ConverterHLSL::Convert(const std::vector<Token>& inTokens, std::vector<Token>& outTokens)
	{
		return false;
		/*
		int curvyBracketDeep = 0;
		bool inMainFuction = false;

		for (uint32_t index = 0; index < inTokens.size(); ++index)
		{
			const ShaderLangToken& token = inTokens[index];

			if (token._type == ShaderLangToken::Type::Identifier
				&& std::get<std::string>(token._data) == "void"
				&& tokens[index + 1]._type == ShaderLangToken::Type::Space
				&& tokens[index + 2]._type == ShaderLangToken::Type::Identifier
				&& std::get<std::string>(tokens[index + 2]._data) == "main"
				&& tokens[index + 3]._type == ShaderLangToken::Type::OpenParenthesis
				&& tokens[index + 4]._type == ShaderLangToken::Type::ClosingParenthesis
				&& tokens[index + 5]._type == ShaderLangToken::Type::Space
				&& tokens[index + 6]._type == ShaderLangToken::Type::OpenCurlyBracket)
			{
				_result += "OUT main(IN in) { OUT out; ";
				index += 4;

				inMainFuction = true;
				continue;
			}
			else if (token._type == ShaderLangToken::Type::ClosingCurlyBracket && inMainFuction == true && curvyBracketDeep == 0)
			{
				_result += "return out; } ";
				inMainFuction = false;
				continue;
			}

			switch (token._type)
			{
				case ShaderLangToken::Type::CBuffer:
				{
					_result += "cbuffer";
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
						if (inMainFuction == true)
						{
							if (value == "IN")
							{
								value = "in";
							}
							else if (value == "OUT")
							{
								value = "out";
							}
						}
						_result += value;
					}
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
		*/
	}
}
