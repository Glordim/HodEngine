#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/FontChain.hpp"

#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>
#include <HodEngine/Text/FontFace.hpp>

namespace hod::inline renderer
{
	/// @brief The fallbacks are loaded here, when the text is drawn: not by the font as it loads, a font falling back on it would be loaded again
	/// @param font
	/// @param fonts
	void FontChain::Append(const std::shared_ptr<FontResource>& font, Vector<std::shared_ptr<FontResource>>& fonts)
	{
		if (font == nullptr || font->GetFontFace().IsLoaded() == false)
		{
			return;
		}
		for (const std::shared_ptr<FontResource>& appended : fonts)
		{
			if (appended == font)
			{
				return;
			}
		}

		fonts.PushBack(font);
		for (const UID& fallback : font->GetFallbacks())
		{
			Append(ResourceManager::GetInstance()->GetResource<FontResource>(fallback), fonts);
		}
	}

	/// @brief
	/// @param font
	/// @return
	bool FontChain::Update(const std::shared_ptr<FontResource>& font)
	{
		_updatedFonts.Clear();
		Append(font, _updatedFonts);

		bool changed = _updatedFonts.Size() != _fonts.Size();
		for (uint32_t index = 0; changed == false && index < _updatedFonts.Size(); ++index)
		{
			const FontResource& updatedFont = *_updatedFonts[index];
			changed = _updatedFonts[index] != _fonts[index] || updatedFont.GetFontFace().GetId() != _faceIds[index] || updatedFont.GetGlyphMode() != _meshFonts[index]._mode ||
			          updatedFont.GetPixelSize() != _meshFonts[index]._pixelSize;
		}
		if (changed == false)
		{
			return false;
		}

		_fonts.Clear();
		_faces.Clear();
		_meshFonts.Clear();
		_faceIds.Clear();
		for (const std::shared_ptr<FontResource>& updatedFont : _updatedFonts)
		{
			_fonts.PushBack(updatedFont);
			_faces.PushBack(&updatedFont->GetFontFace());
			_faceIds.PushBack(updatedFont->GetFontFace().GetId());

			TextMesh::Font& meshFont = _meshFonts.EmplaceBack();
			meshFont._glyphCache = &updatedFont->GetGlyphCache();
			meshFont._mode = updatedFont->GetGlyphMode();
			meshFont._pixelSize = updatedFont->GetPixelSize();
		}
		return true;
	}

	/// @brief
	void FontChain::Clear()
	{
		_fonts.Clear();
		_faces.Clear();
		_meshFonts.Clear();
		_faceIds.Clear();
		_updatedFonts.Clear();
	}

	/// @brief
	/// @return
	bool FontChain::Empty() const
	{
		return _fonts.Empty();
	}

	/// @brief
	/// @return
	const Vector<std::shared_ptr<FontResource>>& FontChain::GetFonts() const
	{
		return _fonts;
	}

	/// @brief
	/// @return
	ArrayView<const FontFace*> FontChain::GetFaces() const
	{
		return _faces;
	}

	/// @brief
	/// @return
	ArrayView<TextMesh::Font> FontChain::GetMeshFonts() const
	{
		return _meshFonts;
	}
}
