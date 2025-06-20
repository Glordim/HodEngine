#include "HodEngine/Renderer/Pch.hpp"
#include "ShaderGeneratorGLSL.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <map>
#include "HodEngine/Core/String.hpp"
#include <string_view>

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/SPIRV/GlslangToSpv.h>

namespace hod::renderer
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
	bool ShaderGeneratorGLSL::ConvertTokens(const Vector<Token>& inTokens, Vector<Token>& outTokens)
	{
		outTokens.emplace_back(Token::Type::Identifier, "#version");
		outTokens.emplace_back(Token::Type::IntegerValue, 450);
		outTokens.emplace_back(Token::Type::Identifier, "core");
		outTokens.emplace_back(Token::Type::NewLine, 0);
		outTokens.emplace_back(Token::Type::NewLine, 0);

		bool inMainFuction = false;
		bool inInStruct = false;
		bool inUboStruct = false;
		bool inOutStruct = false;
		bool inCBufferStruct = false;
		int locationIndex = 0;

		Vector<std::string> identifiers;

		std::map<std::string, std::string> inVariableToQualifier;
		std::map<std::string, std::string> outVariableToQualifier;
		String cbufferIdentifier;

		for (uint32_t index = 0; index < inTokens.size(); ++index)
		{
			const Token& token = inTokens[index];

			if (NextTokensAre(inTokens, index, {{Token::Type::CBuffer, 0}, {Token::Type::Identifier, ""}, {Token::Type::Identifier, ""}, {Token::Type::OpenCurlyBracket, 0}}, &identifiers))
			{
				outTokens.emplace_back(Token::Type::Identifier, "layout");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, "push_constant");
				outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, "uniform");
				outTokens.emplace_back(Token::Type::Identifier, identifiers[0]);
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);
				cbufferIdentifier = identifiers[1];
				inCBufferStruct = true;
				continue;
			}
			else if (inCBufferStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				inCBufferStruct = false;
				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Identifier, cbufferIdentifier);
				outTokens.emplace_back(Token::Type::Semicolon, 0);
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}
			else if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "IN"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inInStruct = true;
				locationIndex = 0;
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

			if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "UBO"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inUboStruct = true;

				outTokens.emplace_back(Token::Type::Identifier, "layout");
				outTokens.emplace_back(Token::Type::OpenParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, "set");
				outTokens.emplace_back(Token::Type::Assign, 0);
				outTokens.emplace_back(Token::Type::IntegerValue, 0);
				outTokens.emplace_back(Token::Type::Comma, 0);
				outTokens.emplace_back(Token::Type::Identifier, "binding");
				outTokens.emplace_back(Token::Type::Assign, 0);
				outTokens.emplace_back(Token::Type::IntegerValue, 0);
				outTokens.emplace_back(Token::Type::ClosingParenthesis, 0);
				outTokens.emplace_back(Token::Type::Identifier, "uniform");
				outTokens.emplace_back(Token::Type::Identifier, "UBO_STRUCT");
				outTokens.emplace_back(Token::Type::OpenCurlyBracket, 0);

				continue;
			}
			else if (inUboStruct == true && NextTokensAre(inTokens, index, {{Token::Type::ClosingCurlyBracket, 0}, {Token::Type::Semicolon, 0}}))
			{
				outTokens.emplace_back(Token::Type::ClosingCurlyBracket, 0);
				outTokens.emplace_back(Token::Type::Identifier, "UBO");
				outTokens.emplace_back(Token::Type::Semicolon, 0);

				inUboStruct = false;
				outTokens.emplace_back(Token::Type::NewLine, 0);
				continue;
			}

			if (NextTokensAre(inTokens, index, {{Token::Type::Struct, 0}, {Token::Type::Identifier, "OUT"}, {Token::Type::OpenCurlyBracket, 0}}))
			{
				inOutStruct = true;
				locationIndex = 0;
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

			if (NextTokensAre(inTokens, index, {{Token::Type::Identifier, "void"}, {Token::Type::Identifier, "VertexMain"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::ClosingParenthesis, 0}, {Token::Type::OpenCurlyBracket, 0}}) ||
				NextTokensAre(inTokens, index, {{Token::Type::Identifier, "void"}, {Token::Type::Identifier, "FragMain"}, {Token::Type::OpenParenthesis, 0}, {Token::Type::ClosingParenthesis, 0}, {Token::Type::OpenCurlyBracket, 0}}))
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
				case Token::Type::Identifier:
				{
					String value = std::get<std::string>(token._data);
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

	/// @brief 
	/// @param byteCode 
	/// @param type 
	/// @param source 
	/// @return 
	bool ShaderGeneratorGLSL::CompileSource(Vector<uint8_t>& byteCode, Shader::ShaderType type, std::string_view source)
	{
		if (glslang::InitializeProcess() == false)
		{
			return false;
		}

		// Chargement et compilation d'un shader
		EShLanguage stage = EShLangVertex; // Par exemple, pour un shader de vertex
		if (type == Shader::ShaderType::Fragment)
		{
			stage = EShLangFragment;
		}
		// todo + swicth

		glslang::TShader shader(stage);
		shader.setEnvInput(glslang::EShSource::EShSourceGlsl, stage, glslang::EShClient::EShClientVulkan, 450);
		shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetVulkan_1_3);
		shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetSpv_1_5);
		std::array<const char*, 1> sources = { source.data() };
		shader.setStrings(sources.data(), (int)sources.size());
		//shader.addSourceText(source.data(), source.size());

		// Compilation GLSL vers SPIR-V
		const TBuiltInResource* defaultResource = GetDefaultResources();
		if (shader.parse(defaultResource, 450, false, EShMsgDefault) == false)
		{
			const char* infoLog = shader.getInfoLog();
    		OUTPUT_ERROR("Shader parsing failed: {}", infoLog);
			return false;
		}

		glslang::TProgram program;
		program.addShader(&shader);
		if (program.link(EShMsgDefault) == false)
		{
			const char* infoLog = program.getInfoLog();
    		OUTPUT_ERROR("Shader linking failed: {}", infoLog);
			return false;
		}

		Vector<uint32_t> spirv;
		glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

		byteCode.resize(spirv.size() * sizeof(uint32_t));
		std::memcpy(byteCode.data(), spirv.data(), byteCode.size());

		glslang::FinalizeProcess();
		return true;
	}
}
