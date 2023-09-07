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
			char current = stream.Peek();
			if (current == ' ' || current == '\r' || current == '\t')
			{
				current.Ignore();
			}
			else if (IsAlphabetic(c) == true)
			{
				std::array<char, 255> tokenName;
				stream.ReadUntil(tokenName, &IsAlphanumeric);
			}
		}
		return true;
	}
}
