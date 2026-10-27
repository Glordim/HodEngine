#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/FontChain.hpp"

#include "HodEngine/Renderer/Resource/FontResource.hpp"

#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>
#include <HodEngine/Text/FontFace.hpp>

namespace hod::inline renderer
{
	/// @brief
	/// @param fonts
	/// @param font
	/// @return
	static bool Contains(const Vector<std::shared_ptr<FontResource>>& fonts, const std::shared_ptr<FontResource>& font)
	{
		for (const std::shared_ptr<FontResource>& contained : fonts)
		{
			if (contained == font)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief The fallbacks and the styles are loaded here, when the text is drawn: not by the font as it loads, a font falling back on it would be
	/// loaded again, and only the styles drawn are
	/// @param font
	/// @param style null: the font itself
	/// @param visited the fonts appended so far, the one of the style of each or itself: a cycle is cut
	/// @param fonts
	void FontChain::Append(const std::shared_ptr<FontResource>& font, const Style* style, Vector<std::shared_ptr<FontResource>>& visited,
	                       Vector<std::shared_ptr<FontResource>>& fonts)
	{
		if (font == nullptr || font->GetFontFace().IsLoaded() == false || Contains(visited, font))
		{
			return;
		}
		visited.PushBack(font);

		// The font of the style and its fallbacks, then the font and its own: what the font of the style lacks is drawn as the font is
		if (style != nullptr)
		{
			const UID styleUid = font->FindStyle(style->_weight, style->_italic);
			if (styleUid.IsValid() && styleUid != font->GetUid())
			{
				std::shared_ptr<FontResource> styleFont = ResourceManager::GetInstance()->GetResource<FontResource>(styleUid);
				if (styleFont != nullptr && styleFont->GetFontFace().IsLoaded() && Contains(fonts, styleFont) == false) // not loaded: the font is drawn
				{
					fonts.PushBack(styleFont);
					for (const UID& fallback : styleFont->GetFallbacks())
					{
						Append(ResourceManager::GetInstance()->GetResource<FontResource>(fallback), style, visited, fonts);
					}
				}
			}
		}

		if (Contains(fonts, font) == false)
		{
			fonts.PushBack(font);
		}
		for (const UID& fallback : font->GetFallbacks())
		{
			Append(ResourceManager::GetInstance()->GetResource<FontResource>(fallback), style, visited, fonts);
		}
	}

	/// @brief
	/// @param font
	/// @return
	bool FontChain::Update(const std::shared_ptr<FontResource>& font)
	{
		return UpdateChain(font, nullptr);
	}

	/// @brief
	/// @param font
	/// @param weight
	/// @param italic
	/// @return
	bool FontChain::Update(const std::shared_ptr<FontResource>& font, FontResource::Weight weight, bool italic)
	{
		const Style style = {weight, italic};
		return UpdateChain(font, &style);
	}

	/// @brief
	/// @param font
	/// @param style null: each font itself
	/// @return
	bool FontChain::UpdateChain(const std::shared_ptr<FontResource>& font, const Style* style)
	{
		_updatedFonts.Clear();
		_visitedFonts.Clear();
		Append(font, style, _visitedFonts, _updatedFonts);

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
		_visitedFonts.Clear();
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
