#include "HodEngine/Text/Pch.hpp"
#include "HodEngine/Text/SampleText.hpp"

#include "HodEngine/Text/FontFace.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <hb.h>

#include <cstring>

namespace hod::inline text
{
	namespace
	{
		struct Range
		{
			char32_t _first;
			char32_t _last;
		};

		struct Script
		{
			const char* _text; // its first two letters
			char32_t    _first;
			char32_t    _second;
			Range       _ranges[3]; // its characters, an empty range ends them
		};

		constexpr Script Latin = {"Aa", U'A', U'a', {{0x0041, 0x005A}, {0x0061, 0x007A}, {0x00C0, 0x024F}}};

		// The other scripts but CJK, the first one with the most characters wins
		constexpr Script Scripts[] = {
			{"\xD0\x91\xD0\xB1", 0x0411, 0x0431, {{0x0400, 0x04FF}}},                 // Бб, Cyrillic
			{"\xCE\x91\xCE\xB1", 0x0391, 0x03B1, {{0x0370, 0x03FF}}},                 // Αα, Greek
			{"\xD8\xB9\xD8\xA8", 0x0639, 0x0628, {{0x0600, 0x06FF}}},                 // عب, Arabic
			{"\xD7\x90\xD7\x91", 0x05D0, 0x05D1, {{0x0590, 0x05FF}}},                 // אב, Hebrew
			{"\xE0\xA4\x85\xE0\xA4\x95", 0x0905, 0x0915, {{0x0900, 0x097F}}},         // अक, Devanagari
			{"\xE0\xB8\x81\xE0\xB8\x82", 0x0E01, 0x0E02, {{0x0E00, 0x0E7F}}},         // กข, Thai
		};

		constexpr Script Chinese = {"\xE4\xB8\xAD\xE6\x96\x87", 0x4E2D, 0x6587, {{0x4E00, 0x9FFF}, {0x3400, 0x4DBF}}}; // 中文
		constexpr Script Japanese = {"\xE3\x81\x82\xE3\x81\x84", 0x3042, 0x3044, {{0x3040, 0x30FF}}};                 // あい
		constexpr Script Korean = {"\xEA\xB0\x80\xEB\x82\x98", 0xAC00, 0xB098, {{0xAC00, 0xD7A3}}};                   // 가나
		constexpr Script Digits = {"12", U'1', U'2', {{0x0030, 0x0039}}};

		constexpr uint32_t CjkCharacterCount = 1000; // ideographs, or Hangul syllables, a CJK font has thousands of

		enum class CjkLanguage : uint8_t
		{
			Unknown,
			Chinese,
			Japanese,
			Korean,
		};

		/// @brief
		/// @param codepoints sorted
		/// @param script
		/// @return
		uint32_t Count(const Vector<char32_t>& codepoints, const Script& script)
		{
			uint32_t count = 0;
			for (char32_t codepoint : codepoints)
			{
				for (const Range& range : script._ranges)
				{
					if (range._last != 0 && codepoint >= range._first && codepoint <= range._last)
					{
						++count;
						break;
					}
				}
			}
			return count;
		}

		/// @brief
		/// @param face
		/// @param script
		/// @return
		bool Has(const FontFace& face, const Script& script)
		{
			return face.HasGlyph(script._first) && face.HasGlyph(script._second);
		}

		/// @brief The usual suffixes of the CJK families: "Noto Sans JP", "Source Han Sans KR", "Noto Sans SC"...
		/// @param familyName
		/// @return
		CjkLanguage FromName(const String& familyName)
		{
			struct Suffix
			{
				const char* _suffix;
				CjkLanguage _language;
			};
			constexpr Suffix Suffixes[] = {
				{" JP", CjkLanguage::Japanese}, {" KR", CjkLanguage::Korean},  {" K", CjkLanguage::Korean},   {" SC", CjkLanguage::Chinese},
				{" TC", CjkLanguage::Chinese},  {" HK", CjkLanguage::Chinese}, {" CN", CjkLanguage::Chinese}, {" J", CjkLanguage::Japanese},
			};

			const char*  name = familyName.CStr();
			const size_t nameLength = std::strlen(name);
			for (const Suffix& suffix : Suffixes)
			{
				const size_t suffixLength = std::strlen(suffix._suffix);
				if (nameLength > suffixLength && std::strcmp(name + nameLength - suffixLength, suffix._suffix) == 0)
				{
					return suffix._language;
				}
			}
			return CjkLanguage::Unknown;
		}

		/// @brief ulCodePageRange1 of the OS/2 table (version 1 and later), when it declares one CJK language only
		/// @param face
		/// @return
		CjkLanguage FromCodePages(const FontFace& face)
		{
			hb_blob_t*   blob = hb_face_reference_table(hb_font_get_face(face.GetHbFont()), HB_TAG('O', 'S', '/', '2'));
			unsigned int size = 0;
			const char*  data = hb_blob_get_data(blob, &size);

			CjkLanguage language = CjkLanguage::Unknown;
			if (data != nullptr && size >= 82)
			{
				const uint8_t* table = reinterpret_cast<const uint8_t*>(data);
				const uint16_t version = (uint16_t)((table[0] << 8) | table[1]);
				const uint32_t codePages = ((uint32_t)table[78] << 24) | ((uint32_t)table[79] << 16) | ((uint32_t)table[80] << 8) | (uint32_t)table[81];
				if (version >= 1)
				{
					const bool japanese = (codePages >> 17) & 1;
					const bool chinese = ((codePages >> 18) & 1) || ((codePages >> 20) & 1);
					const bool korean = ((codePages >> 19) & 1) || ((codePages >> 21) & 1);
					if ((uint32_t)japanese + (uint32_t)chinese + (uint32_t)korean == 1)
					{
						language = japanese ? CjkLanguage::Japanese : (chinese ? CjkLanguage::Chinese : CjkLanguage::Korean);
					}
				}
			}
			hb_blob_destroy(blob);
			return language;
		}

		/// @brief
		/// @param codepoint
		/// @param utf8 at least 4 bytes
		/// @return the bytes written
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

		/// @brief Its first two characters that are not spaces nor controls
		/// @param codepoints
		/// @return
		String FirstCharacters(const Vector<char32_t>& codepoints)
		{
			char     text[9] = {};
			uint32_t length = 0;
			uint32_t count = 0;
			for (char32_t codepoint : codepoints)
			{
				const bool spaceOrControl = codepoint <= 0x20 || (codepoint >= 0x7F && codepoint <= 0xA0) || codepoint == 0x00AD || (codepoint >= 0x2000 && codepoint <= 0x200F);
				if (spaceOrControl == false)
				{
					length += EncodeUtf8(codepoint, text + length);
					if (++count == 2)
					{
						break;
					}
				}
			}
			text[length] = '\0';
			return String(text);
		}
	}

	/// @brief
	/// @param face
	/// @return
	String GetSampleText(const FontFace& face)
	{
		if (face.IsLoaded() == false)
		{
			return String();
		}

		Vector<char32_t> codepoints;
		face.GetCodepoints(codepoints);

		// CJK: thousands of characters, whatever Latin it has too
		const uint32_t hanCount = Count(codepoints, Chinese);
		const uint32_t hangulCount = Count(codepoints, Korean);
		if (hanCount >= CjkCharacterCount || hangulCount >= CjkCharacterCount)
		{
			CjkLanguage language = FromName(face.GetFamilyName());
			if (language == CjkLanguage::Unknown)
			{
				language = FromCodePages(face);
			}
			if (language == CjkLanguage::Unknown)
			{
				language = hangulCount >= CjkCharacterCount ? CjkLanguage::Korean : CjkLanguage::Chinese;
			}

			const Script& script = language == CjkLanguage::Japanese ? Japanese : (language == CjkLanguage::Korean ? Korean : Chinese);
			for (const Script* candidate : {&script, &Chinese, &Japanese, &Korean})
			{
				if (Has(face, *candidate))
				{
					return String(candidate->_text);
				}
			}
		}

		// The script with the most characters, Latin when it has as many
		const Script* best = Has(face, Latin) ? &Latin : nullptr;
		uint32_t      bestCount = best != nullptr ? Count(codepoints, Latin) : 0;
		for (const Script& script : Scripts)
		{
			const uint32_t count = Count(codepoints, script);
			if (count > bestCount && Has(face, script))
			{
				best = &script;
				bestCount = count;
			}
		}
		if (best != nullptr)
		{
			return String(best->_text);
		}

		if (Has(face, Digits))
		{
			return String(Digits._text);
		}
		return FirstCharacters(codepoints);
	}
}
