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
			char32_t	_code = 0;
			float		_advanceX = 0.0f;
			float		_baseline = 0.0f;
			Vector2		_bearing;
			Vector2		_size;
			Vector2		_atlasPos;
			Vector2		_atlasSize;
		};

		struct GlyphGeometry
		{
			Vector2		_posCenter;
			Vector2		_posSize;

			Vector2		_uvPos;
			Vector2		_uvSize;
		};

	public:

		Font();
		~Font();

		bool		LoadFromMemory(const void* data, uint32_t size);

		float		GetKerning(char32_t left, char32_t right) const;

		Texture*			GetTexture() const;
		const GlyphInfo&	GetGlyphInfo(char32_t code) const;

		Vector2				ComputeRequiredSize(const String& value) const;
		void				BuildTextGeometry(const String& value, Vector<GlyphGeometry>& glyphGeometries);

	private:

		FontImpl*				_impl = nullptr;

		Texture*				_texture = nullptr;
		Vector<GlyphInfo>		_glyphInfos;
		GlyphInfo				_unknownGlyphInfo;
	};
}
