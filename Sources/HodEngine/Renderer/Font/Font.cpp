#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Font/Font.hpp"
#include "HodEngine/Renderer/Font/FontImpl.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"

namespace hod::renderer
{
	Font::Font()
	{
		_impl = DefaultAllocator::GetInstance().New<FontImpl>();
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

	Vector2 Font::ComputeRequiredSize(const String& value) const
	{
		return _impl->ComputeRequiredSize(value);
	}

	void Font::BuildTextGeometry(const String& value, Vector<GlyphGeometry>& glyphGeometries)
	{
		glyphGeometries.reserve(value.Size());
		_impl->BuildTextGeometry(value, glyphGeometries, _glyphInfos);
	}
}
