#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/ShaderGeneratorMetal.hpp"

#include <map>
#include <string>
#include <string_view>

namespace hod::renderer
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
		{ "Texture2D", "texture2d<float, access::sample>" },
		{ "Texture3D", "Texture3D" },
		{ "Texture1DArray", "Texture1DArray" },
		{ "Texture2DArray", "Texture2DArray" },
		{ "TextureCube", "TextureCube" },
		{ "SamplerState", "sampler" },
	};

	/// @brief 
	/// @param tokens 
	/// @return 
	bool ConverterMetal::Convert(const std::vector<Token>& inTokens, std::vector<Token>& outTokens)
	{
		outTokens.emplace_back(Token::Type::Identifier, "#include");
		outTokens.emplace_back(Token::Type::Identifier, "<metal_stdlib>");
		outTokens.emplace_back(Token::Type::NewLine, 0);
		outTokens.emplace_back(Token::Type::Identifier, "#include");
		outTokens.emplace_back(Token::Type::Identifier, "<metal_stdlib>");
		outTokens.emplace_back(Token::Type::NewLine, 0);
		outTokens.emplace_back(Token::Type::Identifier, "using");
		outTokens.emplace_back(Token::Type::Identifier, "namespace");
		outTokens.emplace_back(Token::Type::Identifier, "metal");
		outTokens.emplace_back(Token::Type::Semicolon, 0);
		outTokens.emplace_back(Token::Type::NewLine, 0);
		outTokens.emplace_back(Token::Type::NewLine, 0);

		bool inMainFuction = false;
		bool inInStruct = false;
		bool hasInStruct = false;
		bool inOutStruct = false;
		bool inUboStruct = false;
		int locationIndex = 0;
		int bufferIndex = 1;

		std::vector<std::string> identifiers;

		std::map<std::string, std::string> inVariableToQualifier;
		std::map<std::string, std::string> outVariableToQualifier;

		std::vector<Token> mainParameter;

		for (uint32_t index = 0; index < inTokens.size(); ++index)
		{
			const Token& token = inTokens[index];

			if (NextTokensAre(inTokens, index, {{Token::Type::CBuffer, 0}, {Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::OpenCurlyBracket, 0}}, &identifiers))
			{
				outTokens.emplace_back(Token::Type::Struct, 0);
				outTokens.emplace_back(Token::Type::Identifier, identifiers[0]);
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);

				if (mainParameter.empty() == false)
				{
					mainParameter.emplace_back(Token::Type::Comma, 0);
				}
				mainParameter.emplace_back(Token::Type::Identifier, "constant");
				mainParameter.emplace_back(Token::Type::Identifier, identifiers[0]);
				mainParameter.emplace_back(Token::Type::Identifier, "&");
				mainParameter.emplace_back(Token::Type::Identifier, identifiers[1]);
				mainParameter.emplace_back(Token::Type::Identifier, "[[");
				mainParameter.emplace_back(Token::Type::Identifier, "buffer");
				mainParameter.emplace_back(Token::Type::OpenParenthesis, 0);
				mainParameter.emplace_back(Token::Type::IntegerValue, bufferIndex);
				mainParameter.emplace_back(Token::Type::ClosingParenthesis, 0);
				mainParameter.emplace_back(Token::Type::Identifier, "]]");
				++bufferIndex;
				continue;
			}
			else if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "IN"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inInStruct = true;
				hasInStruct = true;
				outTokens.emplace_back(Token::Type::Struct, 0);
				outTokens.emplace_back(Token::Type::Identifier, "IN");
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);
				locationIndex = 0;
				continue;
			}
			else if (inInStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				inInStruct = false;
				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Semicolon, 0);
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}
			else if (inInStruct == true && NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::Colon, 0}, {Token::Type::Identifier, ""}, {Token::Type::Semicolon, 0}}, &identifiers))
			{
				if (identifiers[2] != "SV_POSITION"/* && identifiers[2] != "SV_TARGET"*/)
				{
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

					outTokens.emplace_back(Token::Type::Identifier, "[[");
					outTokens.emplace_back(Token::Type::Identifier, "attribute");
					outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
					outTokens.emplace_back(Token::Type::IntegerValue, locationIndex);
					outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
					outTokens.emplace_back(Token::Type::Identifier, "]]");

					outTokens.emplace_back(Token::Type::Semicolon, 0);

					++locationIndex;
				}
				
				inVariableToQualifier.emplace(identifiers[1], identifiers[2]);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "UBO"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inUboStruct = true;

				outTokens.emplace_back(Token::Type::Struct, 0);
				outTokens.emplace_back(Token::Type::Identifier, "UBO_STRUCT");
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);

				if (mainParameter.empty() == false)
				{
					mainParameter.emplace_back(Token::Type::Comma, 0);
				}
				mainParameter.emplace_back(Token::Type::Identifier, "const device UBO_STRUCT& UBO");
				mainParameter.emplace_back(Token::Type::Identifier, "[[");
				mainParameter.emplace_back(Token::Type::Identifier, "buffer");
				mainParameter.emplace_back(Token::Type::OpenParenthesis, 0);
				mainParameter.emplace_back(Token::Type::IntegerValue, bufferIndex);
				mainParameter.emplace_back(Token::Type::ClosingParenthesis, 0);
				mainParameter.emplace_back(Token::Type::Identifier, "]]");
				++bufferIndex;

				continue;
			}
			else if (inUboStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Semicolon, 0);

				inUboStruct = false;
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "OUT"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inOutStruct = true;
				outTokens.emplace_back(Token::Type::Struct, 0);
				outTokens.emplace_back(Token::Type::Identifier, "OUT");
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);
				locationIndex = 0;
				continue;
			}
			else if (inOutStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				inOutStruct = false;
				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Semicolon, 0);
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}
			
			else if (inOutStruct == true && NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::Colon, 0}, {Token::Type::Identifier, ""}, {Token::Type::Semicolon, 0}}, &identifiers))
			{
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

				if (identifiers[2] == "SV_POSITION")
				{
					outTokens.emplace_back(Token::Type::Identifier, "[[");
					outTokens.emplace_back(Token::Type::Identifier, "position");
					outTokens.emplace_back(Token::Type::Identifier, "]]");
				}
				outTokens.emplace_back(Token::Type::Semicolon, 0);

				++locationIndex;

				outVariableToQualifier.emplace(identifiers[1], identifiers[2]);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, ""}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inOutStruct = true;
				outTokens.emplace_back(Token::Type::Struct, 0);
				outTokens.emplace_back(Token::Type::Identifier, "OUT");
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);
				locationIndex = 0;
				continue;
			}
			else if (inOutStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				inOutStruct = false;
				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Semicolon, 0);
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::Colon, 0}, {Token::Type::Identifier, "register"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::Identifier, ""}, {Token::Type::ClosingParenthesis, 0}, {Token::Type::Semicolon, 0}}, &identifiers))
			{
				const char* attributeType = nullptr;
				if (identifiers[3][0] == 't') // texture
				{
					attributeType = "texture";
				}
				else if (identifiers[3][0] == 's') // sampler
				{
					attributeType = "sampler";
				}

				int index = std::atoi(identifiers[3].c_str() + 1);

				if (mainParameter.empty() == false)
				{
					mainParameter.emplace_back(Token::Type::Comma, 0);
				}

				auto it = _identifierMap.find(identifiers[0]);
				if (it != _identifierMap.end())
				{
					mainParameter.emplace_back(Token::Type::Identifier, std::string(it->second));
				}
				else
				{
					return false;
				}
				mainParameter.emplace_back(Token::Type::Identifier, identifiers[1]);
				mainParameter.emplace_back(Token::Type::Identifier, "[[");
				mainParameter.emplace_back(Token::Type::Identifier, attributeType);
				mainParameter.emplace_back(Token::Type::OpenParenthesis, 0);
				mainParameter.emplace_back(Token::Type::IntegerValue, index);
				mainParameter.emplace_back(Token::Type::ClosingParenthesis, 0);
				mainParameter.emplace_back(Token::Type::Identifier, "]]");
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, ""}, {Token::Type::Dot, 0}, {Token::Type::Identifier, "Sample"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::Identifier, ""}}, &identifiers))
			{
				outTokens.emplace_back(Token::Type::Identifier, identifiers[0]);
				outTokens.emplace_back(Token::Type::Dot, 0);
				outTokens.emplace_back(Token::Type::Identifier, "sample");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, identifiers[2]);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, "void"}, {Token::Type::Identifier, "VertexMain"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::ClosingParenthesis, 0}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				outTokens.emplace_back(Token::Type::Identifier, "vertex");
				outTokens.emplace_back(Token::Type::Identifier, "OUT");
				outTokens.emplace_back(Token::Type::Identifier, "VertexMain");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				if (hasInStruct)
				{
					outTokens.emplace_back(Token::Type::Identifier, "IN");
					outTokens.emplace_back(Token::Type::Identifier, "in");
					outTokens.emplace_back(Token::Type::Identifier, "[[");
					outTokens.emplace_back(Token::Type::Identifier, "stage_in");
					outTokens.emplace_back(Token::Type::Identifier, "]]");
				}
				if (mainParameter.empty() == false)
				{
					if (hasInStruct)
					{
						outTokens.emplace_back(Token::Type::Comma, 0);
					}
					outTokens.insert(outTokens.end(), mainParameter.begin(), mainParameter.end());
				}
				outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Identifier, "OUT");
				outTokens.emplace_back(Token::Type::Identifier, "out");
				outTokens.emplace_back(Token::Type::Semicolon, 0);
				outTokens.emplace_back(Token::Type::NewLine, 0);

				inMainFuction = true;
				continue;
			}
			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, "void"}, {Token::Type::Identifier, "FragMain"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::ClosingParenthesis, 0}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				outTokens.emplace_back(Token::Type::Identifier, "fragment");
				outTokens.emplace_back(Token::Type::Identifier, "OUT");
				outTokens.emplace_back(Token::Type::Identifier, "FragMain");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				if (hasInStruct)
				{
					outTokens.emplace_back(Token::Type::Identifier, "IN");
					outTokens.emplace_back(Token::Type::Identifier, "in");
					outTokens.emplace_back(Token::Type::Identifier, "[[");
					outTokens.emplace_back(Token::Type::Identifier, "stage_in");
					outTokens.emplace_back(Token::Type::Identifier, "]]");
				}
				if (mainParameter.empty() == false)
				{
					if (hasInStruct)
					{
						outTokens.emplace_back(Token::Type::Comma, 0);
					}
					outTokens.insert(outTokens.end(), mainParameter.begin(), mainParameter.end());
				}
				outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Identifier, "OUT");
				outTokens.emplace_back(Token::Type::Identifier, "out");
				outTokens.emplace_back(Token::Type::Semicolon, 0);
				outTokens.emplace_back(Token::Type::NewLine, 0);

				inMainFuction = true;
				continue;
			}
			else if (inMainFuction == true && token._type == Token::Type::ClosingCurlyBracket)
			{
				outTokens.emplace_back(Token::Type::Return, 0);
				outTokens.emplace_back(Token::Type::Identifier, "out");
				outTokens.emplace_back(Token::Type::Semicolon, 0);

				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				inMainFuction = false;
				continue;
			}
			else if (inMainFuction == true && NextTokensAre(inTokens, index, {{Token::Type::Identifier, "IN"}, {Token::Type::Dot, 0}, {Token::Type::Identifier, ""}}, &identifiers))
			{
				auto it = inVariableToQualifier.find(identifiers[1]);
				if (it != inVariableToQualifier.end())
				{
					outTokens.emplace_back(Token::Type::Identifier, "in");
					outTokens.emplace_back(Token::Type::Dot, 0);
					outTokens.emplace_back(Token::Type::Identifier, it->first);
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
					outTokens.emplace_back(Token::Type::Identifier, "out");
					outTokens.emplace_back(Token::Type::Dot, 0);
					outTokens.emplace_back(Token::Type::Identifier, it->first);
				}
				else
				{
					return false;
				}
				continue;
			}

			switch (token._type)
			{
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
