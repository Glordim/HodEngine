#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/FontChain.hpp"

#include "HodEngine/Renderer/Resource/FontFaceResource.hpp"

#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <utility>

namespace hod::inline renderer
{
	/// @brief
	/// @param resources
	/// @param resource
	/// @return
	template<typename _Resource_>
	static bool Contains(const Vector<std::shared_ptr<_Resource_>>& resources, const std::shared_ptr<_Resource_>& resource)
	{
		for (const std::shared_ptr<_Resource_>& contained : resources)
		{
			if (contained == resource)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief Loaded here, when the text is drawn: only the faces drawn are
	/// @param face
	/// @param faces
	void FontChain::AppendFace(const UID& face, Vector<std::shared_ptr<FontFaceResource>>& faces)
	{
		if (face.IsValid() == false)
		{
			return;
		}
		std::shared_ptr<FontFaceResource> faceResource = ResourceManager::GetInstance()->GetResource<FontFaceResource>(face);
		if (faceResource != nullptr && faceResource->GetFontFace().IsLoaded() && Contains(faces, faceResource) == false)
		{
			faces.PushBack(faceResource);
		}
	}

	/// @brief The fallbacks are loaded here, when the text is drawn: not by the font as it loads, a font falling back on it would be loaded again
	/// @param font
	/// @param weight
	/// @param italic
	/// @param fonts the ones appended so far: a cycle is cut
	/// @param faces
	void FontChain::Append(const std::shared_ptr<FontResource>& font, FontResource::Weight weight, bool italic, Vector<std::shared_ptr<FontResource>>& fonts,
	                       Vector<std::shared_ptr<FontFaceResource>>& faces)
	{
		if (font == nullptr || Contains(fonts, font))
		{
			return;
		}
		fonts.PushBack(font);

		AppendFace(font->FindStyle(weight, italic), faces);
		AppendFace(font->FindStyle(FontResource::Weight::Regular, false), faces); // what the face of the style lacks, drawn as in the rest of the text
		for (const UID& fallback : font->GetFallbacks())
		{
			Append(ResourceManager::GetInstance()->GetResource<FontResource>(fallback), weight, italic, fonts, faces);
		}
	}

	/// @brief
	/// @param font
	/// @param weight
	/// @param italic
	/// @return
	bool FontChain::Update(const std::shared_ptr<FontResource>& font, FontResource::Weight weight, bool italic)
	{
		_updatedFonts.Clear();
		_updatedFaces.Clear();
		Append(font, weight, italic, _updatedFonts, _updatedFaces);
		std::swap(_fonts, _updatedFonts); // the fallbacks not drawn anymore are let go of, after the faces they kept loaded are found again
		return UpdateFaces();
	}

	/// @brief
	/// @param face
	/// @return
	bool FontChain::Update(const std::shared_ptr<FontFaceResource>& face)
	{
		_updatedFaces.Clear();
		if (face != nullptr && face->GetFontFace().IsLoaded())
		{
			_updatedFaces.PushBack(face);
		}
		_fonts.Clear();
		return UpdateFaces();
	}

	/// @brief
	/// @return
	bool FontChain::UpdateFaces()
	{
		bool changed = _updatedFaces.Size() != _faceResources.Size();
		for (uint32_t index = 0; changed == false && index < _updatedFaces.Size(); ++index)
		{
			const FontFaceResource& updatedFace = *_updatedFaces[index];
			changed = _updatedFaces[index] != _faceResources[index] || updatedFace.GetFontFace().GetId() != _faceIds[index] ||
			          updatedFace.GetGlyphMode() != _meshFonts[index]._mode || updatedFace.GetPixelSize() != _meshFonts[index]._pixelSize;
		}
		if (changed == false)
		{
			return false;
		}

		_faceResources.Clear();
		_faces.Clear();
		_meshFonts.Clear();
		_faceIds.Clear();
		for (const std::shared_ptr<FontFaceResource>& updatedFace : _updatedFaces)
		{
			_faceResources.PushBack(updatedFace);
			_faces.PushBack(&updatedFace->GetFontFace());
			_faceIds.PushBack(updatedFace->GetFontFace().GetId());

			TextMesh::Font& meshFont = _meshFonts.EmplaceBack();
			meshFont._glyphCache = &updatedFace->GetGlyphCache();
			meshFont._mode = updatedFace->GetGlyphMode();
			meshFont._pixelSize = updatedFace->GetPixelSize();
		}
		return true;
	}

	/// @brief
	void FontChain::Clear()
	{
		_faceResources.Clear();
		_faces.Clear();
		_meshFonts.Clear();
		_faceIds.Clear();
		_fonts.Clear();
		_updatedFaces.Clear();
		_updatedFonts.Clear();
	}

	/// @brief
	/// @return
	bool FontChain::Empty() const
	{
		return _faceResources.Empty();
	}

	/// @brief
	/// @return
	const Vector<std::shared_ptr<FontFaceResource>>& FontChain::GetFaceResources() const
	{
		return _faceResources;
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
