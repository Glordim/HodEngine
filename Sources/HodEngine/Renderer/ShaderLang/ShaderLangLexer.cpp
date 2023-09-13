#include "HodEngine/Renderer/ShaderLang/ShaderLangLexer.h"

#include "HodEngine/Core/CharHelper.h"
#include "HodEngine/Core/Output.h"
#include "HodEngine/Core/Stream/Stream.h"

#include <array>

namespace hod::renderer
{
	bool ShaderLangLexer::Tokenize(Stream& stream, std::vector<ShaderLangToken>& tokens)
	{
		if (stream.CanRead() == false)
		{
			OUTPUT_ERROR("ShaderLangLexer: Unable to read stream");
			return false;
		}

		while (true)
		{
			char c = stream.Peek();
			if (IsWhitespace(c) == true)
			{
				stream.Ignore();
			}
			else if (IsAlphabetic(c) == true)
			{
				std::array<char, 255> tokenBuffer;
				if (stream.ReadUntil(tokenBuffer.data(), tokenBuffer.max_size(), &IsAlphanumeric) == false)
				{
					OUTPUT_ERROR("ShaderLangLexer: Invalid token (too long)");
					return false;
				}
				std::string_view tokenName(tokenBuffer.data());
				if (tokenName == "struct")
				{
					ShaderLangToken token;
					token._type = ShaderLangToken::Struct;
					tokens.push_back(token);
				}
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
		}
		return true;
	}
}
