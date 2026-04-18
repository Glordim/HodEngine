#pragma once
#include "HodEngine/Renderer/Font/Font.hpp"

#include <freetype/freetype.h>

namespace hod::renderer
{
	class FontImpl final
	{
	public:

					~FontImpl();

		bool		LoadFromMemory(const void* data, uint32_t size, Texture*& texture, Vector<Font::GlyphInfo>& glyphInfos);

		float		GetKerning(char32_t left, char32_t right) const;

		Vector2		ComputeRequiredSize(const String& value) const;
		void		BuildTextGeometry(const String& value, Vector<Font::GlyphGeometry>& glyphGeometries, const Vector<Font::GlyphInfo>& glyphInfos);
	
	private:

		FT_Face		_ftFace = nullptr;
		void*		_data = nullptr;
	};
}
