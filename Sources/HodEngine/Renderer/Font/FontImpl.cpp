#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Font/FontImpl.hpp"
#include "HodEngine/Renderer/Font/FontManager.hpp"
#include "HodEngine/Renderer/Font/FontManagerImpl.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

namespace hod::renderer
{
		// Fonction pour obtenir l'explication d'un code FT_Error
	const char* FT_Error_String(FT_Error err) {
		#undef FTERRORS_H_
		#define FT_ERRORDEF(e, v, s) case e: return s;
		#define FT_ERROR_START_LIST switch (err) {
		#define FT_ERROR_END_LIST }
		#include FT_ERRORS_H
		return "(Unknown error)";
	}

	void embolden_glyph(FT_GlyphSlot slot, FT_Pos strength)
	{
		FT_Outline *outline = &slot->outline;
		for (int i = 0; i < outline->n_points; i++) {
			outline->points[i].x += (outline->points[i].x > 0) ? strength : -strength;
			outline->points[i].y += (outline->points[i].y > 0) ? strength : -strength;
		}
	}

	// Fonction pour rendre un glyphe en gras
	void render_glyph_bold(FT_GlyphSlot slot)
	{
		FT_Pos strength = 1 << 6; // Ajustez cette valeur pour l'épaisseur
		embolden_glyph(slot, strength);
	}

	// Fonction pour rendre un glyphe en italique
	void render_glyph_italic(FT_GlyphSlot slot)
	{
		FT_Matrix matrix;
		matrix.xx = 0x10000;
		matrix.xy = 0x6000; // Ajustez cette valeur pour l'inclinaison
		matrix.yx = 0x0000;
		matrix.yy = 0x10000;

		//FT_Outline_Transform(&slot->outline, &matrix);
	}

	unsigned int nextPowerOf2(unsigned int n)
	{
		if ((n & (n - 1)) == 0) return n;

		unsigned int power = 1;
		while (power < n) {
			power <<= 1;
		}
		return power;
	}

	/// @brief 
	FontImpl::~FontImpl()
	{
		FT_Done_Face(_ftFace);
	}

	/// @brief 
	/// @param data 
	/// @param size 
	/// @return 
	bool FontImpl::LoadFromMemory(const void* data, uint32_t size, Texture*& texture, Vector<Font::GlyphInfo>& glyphInfos)
	{
		FT_Library ftLibrary = FontManager::GetInstance()->GetImpl()->GetFtLibrary();
		FT_Error error = FT_New_Memory_Face(ftLibrary, static_cast<const FT_Byte*>(data), size, 0, &_ftFace);
		if (error != 0)
		{
			OUTPUT_ERROR("FreeType::FT_New_Memory_Face {}", FT_Error_String(error));
			return false;
		}

		error = FT_Select_Charmap(_ftFace, FT_ENCODING_UNICODE);
		if (error != 0)
		{
			FT_Done_Face(_ftFace);
			_ftFace = nullptr;
			OUTPUT_ERROR("FreeType::FT_Select_Charmap {}", FT_Error_String(error));
			return false;
		}

		error = FT_Set_Pixel_Sizes(_ftFace, 0, 48);
		if (error != 0)
		{
			FT_Done_Face(_ftFace);
			_ftFace = nullptr;
			OUTPUT_ERROR("FreeType::FT_Set_Pixel_Sizes {}", FT_Error_String(error));
			return false;
		}

		struct CharacterData
		{
			uint8_t* pixels = nullptr;
			uint32_t width = 0;
			uint32_t height = 0;
		};

		Vector<CharacterData> characterDatas;
		characterDatas.reserve(128);

		glyphInfos.reserve(128);

		for (uint8_t c = 0; c < 128; ++c)
		{
			FT_UInt charIndex = FT_Get_Char_Index(_ftFace, c);
			if (charIndex == 0)
			{
				continue;
			}

			error = FT_Load_Glyph(_ftFace, charIndex, FT_LOAD_DEFAULT);
			if (error != 0)
			{
				OUTPUT_ERROR("FreeType::FT_Load_Glyph {}", FT_Error_String(error));
				FT_Done_Face(_ftFace);
				_ftFace = nullptr;
				return false;
			}

			// Render the glyph to a bitmap
			error = FT_Render_Glyph(_ftFace->glyph, FT_RENDER_MODE_NORMAL);
			if (error != 0)
			{
				OUTPUT_ERROR("FreeType::FT_Render_Glyph {}", FT_Error_String(error));
				FT_Done_Face(_ftFace);
				_ftFace = nullptr;
				return false;
			}

			FT_Bitmap ftBitmap = _ftFace->glyph->bitmap;
			if (ftBitmap.buffer != nullptr && ftBitmap.rows > 0 && ftBitmap.width > 0)
			{
				CharacterData characterData;
				characterData.pixels = DefaultAllocator::GetInstance().Allocate<uint8_t>(ftBitmap.rows * ftBitmap.width * 4);
				characterData.width = ftBitmap.width;
				characterData.height = ftBitmap.rows;
				for (uint32_t y = 0; y < ftBitmap.rows; y++)
				{
					for (uint32_t x = 0; x < ftBitmap.width; x++)
					{
						unsigned char pixel = ftBitmap.buffer[y * ftBitmap.pitch + x];
						characterData.pixels[y * ftBitmap.width * 4 + x * 4] = 255;
						characterData.pixels[y * ftBitmap.width * 4 + x * 4 + 1] = 255;
						characterData.pixels[y * ftBitmap.width * 4 + x * 4 + 2] = 255;
						characterData.pixels[y * ftBitmap.width * 4 + x * 4 + 3] = pixel;
					}
				}

				characterDatas.push_back(characterData);

				Font::GlyphInfo glyphInfo;
				glyphInfo._code = c;
				glyphInfo._advance = Vector2(float(_ftFace->glyph->advance.x >> 6), float(_ftFace->glyph->advance.y >> 6));
				glyphInfo._baseline = 48 - _ftFace->glyph->bitmap_top;
				glyphInfo._size = Vector2((float)ftBitmap.width, (float)ftBitmap.rows);
				glyphInfo._offsetX = 0;
				glyphInfo._offsetY = 0;
				glyphInfos.push_back(glyphInfo);
			}
		}

		uint32_t characterByLine = (uint32_t)std::sqrt(characterDatas.size()) + 1;
		uint32_t atlasWidth = nextPowerOf2(characterByLine * 48);
		uint8_t* atlas = DefaultAllocator::GetInstance().Allocate<uint8_t>(atlasWidth * atlasWidth * 4);
		for (uint32_t characteIndex = 0; characteIndex < characterDatas.size(); ++characteIndex)
		{
			const CharacterData& characterData = characterDatas[characteIndex];
			Font::GlyphInfo& glyphInfo = glyphInfos[characteIndex];
			glyphInfo._offsetX = (characteIndex % characterByLine) * 48;
			glyphInfo._offsetY = (characteIndex / characterByLine) * 48;

			uint32_t lineOffset = (characteIndex / characterByLine) * atlasWidth * 4 * 48;

			for (uint32_t y = 0; y < characterData.height; ++y)
			{
				std::memcpy(atlas + lineOffset + (characteIndex % characterByLine) * 48 * 4 + y * atlasWidth * 4, characterData.pixels + y * characterData.width * 4, 4 * characterData.width);
			}
		}
		for (const CharacterData& characterData : characterDatas)
		{
			DefaultAllocator::GetInstance().Free(characterData.pixels);
		}

		Texture::CreateInfo createInfo;
		texture = Renderer::GetInstance()->CreateTexture();
		if (texture->BuildBuffer(atlasWidth, atlasWidth, (unsigned char*)atlas, createInfo) == false) // todo BuildBuffer doesn't take void* ?
		{
			DefaultAllocator::GetInstance().Free(atlas);
			
			DefaultAllocator::GetInstance().Delete(texture);
			texture = nullptr;

			FT_Done_Face(_ftFace);
			_ftFace = nullptr;

			return false;
		}
		DefaultAllocator::GetInstance().Free(atlas);

		return true;
	}

	/// @brief 
	/// @param left 
	/// @param right 
	/// @return 
	float FontImpl::GetKerning(char32_t left, char32_t right) const
	{
		FT_UInt leftCharIndex = FT_Get_Char_Index(_ftFace, left);
		FT_UInt rightCharIndex = FT_Get_Char_Index(_ftFace, right);

		FT_Vector delta;
		FT_Get_Kerning(_ftFace, leftCharIndex, rightCharIndex, FT_KERNING_DEFAULT, &delta);

		return float(delta.x >> 6);
	}
}
