#include "HodEngine/Text/Pch.hpp"
#include "HodEngine/Text/Unicode.hpp"

#include <hb.h>

namespace hod::inline text
{
	/// @brief From the Unicode data of HarfBuzz
	/// @param codepoint
	/// @return
	bool IsDrawnCharacter(char32_t codepoint)
	{
		switch (hb_unicode_general_category(hb_unicode_funcs_get_default(), codepoint))
		{
			case HB_UNICODE_GENERAL_CATEGORY_UNASSIGNED:
			case HB_UNICODE_GENERAL_CATEGORY_CONTROL:
			case HB_UNICODE_GENERAL_CATEGORY_FORMAT:
			case HB_UNICODE_GENERAL_CATEGORY_SURROGATE:
			case HB_UNICODE_GENERAL_CATEGORY_SPACE_SEPARATOR:
			case HB_UNICODE_GENERAL_CATEGORY_LINE_SEPARATOR:
			case HB_UNICODE_GENERAL_CATEGORY_PARAGRAPH_SEPARATOR: return false;
			default: return true;
		}
	}

	/// @brief
	/// @param codepoint
	/// @param utf8
	/// @return
	uint32_t EncodeUtf8(char32_t codepoint, char* utf8)
	{
		if (codepoint < 0x80)
		{
			utf8[0] = (char)codepoint;
			return 1;
		}
		if (codepoint < 0x800)
		{
			utf8[0] = (char)(0xC0 | (codepoint >> 6));
			utf8[1] = (char)(0x80 | (codepoint & 0x3F));
			return 2;
		}
		if (codepoint < 0x10000)
		{
			utf8[0] = (char)(0xE0 | (codepoint >> 12));
			utf8[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
			utf8[2] = (char)(0x80 | (codepoint & 0x3F));
			return 3;
		}
		utf8[0] = (char)(0xF0 | (codepoint >> 18));
		utf8[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
		utf8[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
		utf8[3] = (char)(0x80 | (codepoint & 0x3F));
		return 4;
	}

	/// @brief Overlong forms, surrogates and values past U+10FFFF are invalid too
	/// @param utf8
	/// @param codepoints
	void DecodeUtf8(StringView utf8, Vector<char32_t>& codepoints)
	{
		constexpr char32_t Replacement = 0xFFFD;

		uint32_t index = 0;
		while (index < utf8.Size())
		{
			const uint8_t first = (uint8_t)utf8[index];

			uint32_t length = 0;
			char32_t codepoint = 0;
			char32_t minimum = 0;
			if (first < 0x80)
			{
				length = 1;
				codepoint = first;
			}
			else if ((first & 0xE0) == 0xC0)
			{
				length = 2;
				codepoint = first & 0x1F;
				minimum = 0x80;
			}
			else if ((first & 0xF0) == 0xE0)
			{
				length = 3;
				codepoint = first & 0x0F;
				minimum = 0x800;
			}
			else if ((first & 0xF8) == 0xF0)
			{
				length = 4;
				codepoint = first & 0x07;
				minimum = 0x10000;
			}

			bool valid = length != 0 && index + length <= utf8.Size();
			for (uint32_t continuation = 1; valid && continuation < length; ++continuation)
			{
				const uint8_t byte = (uint8_t)utf8[index + continuation];
				valid = (byte & 0xC0) == 0x80;
				codepoint = (codepoint << 6) | (byte & 0x3F);
			}
			valid = valid && codepoint >= minimum && codepoint <= 0x10FFFF && (codepoint < 0xD800 || codepoint > 0xDFFF);

			if (valid)
			{
				codepoints.PushBack(codepoint);
				index += length;
			}
			else
			{
				codepoints.PushBack(Replacement);
				++index;
			}
		}
	}
}
