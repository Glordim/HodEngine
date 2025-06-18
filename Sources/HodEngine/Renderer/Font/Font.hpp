#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod::renderer
{
	class Texture;
	class FontImpl;

	class HOD_RENDERER_API Font final
	{
	public:

		struct GlyphInfo
		{
			char32_t	_code;
			uint32_t	_offsetX;
			uint32_t	_offsetY;
			uint32_t	_baseline;
			Vector2		_size;
			Vector2		_advance;
		};

	public:

		Font();
		~Font();

		bool		LoadFromMemory(const void* data, uint32_t size);

		float		GetKerning(char32_t left, char32_t right) const;

		Texture*			GetTexture() const;
		const GlyphInfo&	GetGlyphInfo(char32_t code) const;

	private:

		FontImpl*				_impl = nullptr;

		Texture*				_texture = nullptr;
		Vector<GlyphInfo>		_glyphInfos;
		GlyphInfo				_unknownGlyphInfo;
	};
}
