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
				std::array<char, 255> tokenName;
				if (stream.ReadUntil(tokenName.data(), tokenName.max_size(), &IsAlphanumeric) == false)
				{
					OUTPUT_ERROR("ShaderLangLexer: Invalid token (too long)");
					return false;
				}
			}
		}
		return true;
	}
}
