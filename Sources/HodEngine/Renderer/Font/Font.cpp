#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Font/Font.hpp"
#include "HodEngine/Renderer/Font/FontImpl.hpp"

namespace hod::renderer
{
	Font::Font()
	{
		_impl = DefaultAllocator::GetInstance().New<FontImpl>();

		_unknownGlyphInfo._code = 0;
		_unknownGlyphInfo._baseline = 0;
		_unknownGlyphInfo._offsetX = 0;
		_unknownGlyphInfo._offsetY = 0;
		_unknownGlyphInfo._size = Vector2::Zero;
	}
	
	Font::~Font()
	{
		DefaultAllocator::GetInstance().Delete(_texture);
		DefaultAllocator::GetInstance().Delete(_impl);
	}

	bool Font::LoadFromMemory(const void* data, uint32_t size)
	{
		return _impl->LoadFromMemory(data, size, _texture, _glyphInfos);
	}

	float Font::GetKerning(char32_t left, char32_t right) const
	{
		return _impl->GetKerning(left, right);
	}

	/// @brief 
	/// @return 
	Texture* Font::GetTexture() const
	{
		return _texture;
	}

	/// @brief 
	/// @param code 
	/// @return 
	const Font::GlyphInfo& Font::GetGlyphInfo(char32_t code) const
	{
		for (const GlyphInfo& glyphInfo : _glyphInfos)
		{
			if (glyphInfo._code == code)
			{
				return glyphInfo;
			}
		}
		return _unknownGlyphInfo;
	}
}
