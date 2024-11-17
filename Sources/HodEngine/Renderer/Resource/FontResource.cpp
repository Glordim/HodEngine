#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/FontResource.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <freetype/freetype.h>
#include <cmath>
#include <cstring>

namespace hod::renderer
{
	DESCRIBE_REFLECTED_CLASS(FontResource, reflectionDescriptor)
	{
	}

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
	FontResource::~FontResource()
	{
		delete _texture;
	}

	/// @brief 
	/// @param document 
	/// @param stream 
	/// @return 
	bool FontResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}
		
		const Document::Node* dataOffsetNode = documentNode.GetChild("DataOffset");
		if (dataOffsetNode == nullptr)
		{
			// TODO message
			return false;
		}

		uint32_t initialStreamPos = FileSystem::GetInstance()->GetOffset(fileHandle);
		uint32_t dataOffset = dataOffsetNode->GetUInt32();

		const Document::Node* dataSizeNode = documentNode.GetChild("DataSize");
		if (dataSizeNode == nullptr)
		{
			// TODO message
			return false;
		}

		uint32_t dataSize = dataSizeNode->GetUInt32();

		FileSystem::GetInstance()->Seek(fileHandle, initialStreamPos + dataOffset, FileSystem::SeekMode::Begin);

		FT_Error error;
		FT_Library ftLibrary;
    	error = FT_Init_FreeType(&ftLibrary);
    	if (error != 0)
		{
			OUTPUT_ERROR("FreeType::FT_Init_FreeType {}", FT_Error_String(error));
			return false;
		}

		uint8_t* data = new uint8_t[dataSize];
		if (FileSystem::GetInstance()->Read(fileHandle, (char*)data, dataSize) != dataSize)
		{
			delete[] data;
			return false;
		}

		FT_Face ftFace;
		error = FT_New_Memory_Face(ftLibrary, data, dataSize, 0, &ftFace);

		if (error != 0)
		{
			delete[] data;
			FT_Done_FreeType(ftLibrary);
			OUTPUT_ERROR("FreeType::FT_New_Memory_Face {}", FT_Error_String(error));
			return false;
		}

		error = FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);
		if (error != 0)
		{
			FT_Done_Face(ftFace);
			delete[] data;
			FT_Done_FreeType(ftLibrary);
			OUTPUT_ERROR("FreeType::FT_Select_Charmap {}", FT_Error_String(error));
			return false;
		}

    	error = FT_Set_Pixel_Sizes(ftFace, 0, 48);
		if (error != 0)
		{
			OUTPUT_ERROR("FreeType::FT_Set_Pixel_Sizes {}", FT_Error_String(error));
			FT_Done_Face(ftFace);
			delete[] data;
			FT_Done_FreeType(ftLibrary);
			return false;
		}

		struct CharacterData
		{
			uint8_t* pixels = nullptr;
			uint32_t width = 0;
			uint32_t height = 0;
		};

		std::vector<CharacterData> characterDatas;
		characterDatas.reserve(128);

		_kernings.reserve(128);
		_unknownKerning._code = 0;
		_unknownKerning._baseline = 0;
		_unknownKerning._offsetX = 0;
		_unknownKerning._offsetY = 0;
		_unknownKerning._sizeX = 0;
		_unknownKerning._sizeY = 0;

		for (uint8_t c = 0; c < 128; ++c)
		{
			FT_UInt charIndex = FT_Get_Char_Index(ftFace, c);
			if (charIndex == 0)
			{
				continue;
			}

			error = FT_Load_Glyph(ftFace, charIndex, FT_LOAD_DEFAULT);
			if (error != 0)
			{
				OUTPUT_ERROR("FreeType::FT_Load_Glyph {}", FT_Error_String(error));
				FT_Done_Face(ftFace);
				delete[] data;
				FT_Done_FreeType(ftLibrary);
				return false;
			}

			// Render the glyph to a bitmap
			error = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
			if (error != 0)
			{
				OUTPUT_ERROR("FreeType::FT_Render_Glyph {}", FT_Error_String(error));
				FT_Done_Face(ftFace);
				delete[] data;
				FT_Done_FreeType(ftLibrary);
				return false;
			}

			FT_Bitmap ftBitmap = ftFace->glyph->bitmap;
			if (ftBitmap.buffer != nullptr && ftBitmap.rows > 0 && ftBitmap.width > 0)
			{
				CharacterData characterData;
				characterData.pixels = new uint8_t[ftBitmap.rows * ftBitmap.width * 4];
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

				Kerning kerning;
				kerning._code = c;
				kerning._baseline = 48 - ftFace->glyph->bitmap_top;
				kerning._sizeX = ftBitmap.width;
				kerning._sizeY = ftBitmap.rows;
				kerning._offsetX = 0;
				kerning._offsetY = 0;
				_kernings.push_back(kerning);
			}
		}

		FT_Done_Face(ftFace);
		delete[] data;
		FT_Done_FreeType(ftLibrary);

		uint32_t characterByLine = (uint32_t)std::sqrt(characterDatas.size()) + 1;
		uint32_t atlasWidth = nextPowerOf2(characterByLine * 48);
		uint8_t* atlas = new uint8_t[atlasWidth * atlasWidth * 4];
		for (uint32_t characteIndex = 0; characteIndex < characterDatas.size(); ++characteIndex)
		{
			const CharacterData& characterData = characterDatas[characteIndex];
			Kerning& kerning = _kernings[characteIndex];
			kerning._offsetX = (characteIndex % characterByLine) * 48;
			kerning._offsetY = (characteIndex / characterByLine) * 48;

			uint32_t lineOffset = (characteIndex / characterByLine) * atlasWidth * 4 * 48;

			for (uint32_t y = 0; y < characterData.height; ++y)
			{
				std::memcpy(atlas + lineOffset + (characteIndex % characterByLine) * 48 * 4 + y * atlasWidth * 4, characterData.pixels + y * characterData.width * 4, 4 * characterData.width);
			}
		}
		for (const CharacterData& characterData : characterDatas)
		{
			delete[] characterData.pixels;
		}

		Texture::CreateInfo createInfo;
		_texture = Renderer::GetInstance()->CreateTexture();
		if (_texture->BuildBuffer(atlasWidth, atlasWidth, (unsigned char*)atlas, createInfo) == false) // todo BuildBuffer doesn't take void* ?
		{
			delete[] atlas;
			
			delete _texture;
			_texture = nullptr;

			return false;
		}
		delete[] atlas;
		
		return true;
	}

	/// @brief 
	/// @return 
	Texture* FontResource::GetTexture() const
	{
		return _texture;
	}

	/// @brief 
	/// @param code 
	/// @return 
	const FontResource::Kerning& FontResource::GetKerning(char32_t code) const
	{
		for (const Kerning& kerning : _kernings)
		{
			if (kerning._code == code)
			{
				return kerning;
			}
		}
		return _unknownKerning;
	}
}
