#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontHelper.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontThumbnail.hpp"
#include "HodEngine/Editor/Thumbnail.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Hash.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>
#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Text/FontFace.hpp>

#include <algorithm>
#include <utility>

namespace hod::inline editor
{
	/// @brief
	/// @param face
	/// @return
	FontSettings::Style FontHelper::DetectStyle(const FontFace& face)
	{
		const uint32_t weight = face.GetWeight();

		FontSettings::Style style;
		style._weight = weight == 0 ? FontResource::Weight::Regular : static_cast<FontResource::Weight>(Clamp<uint32_t>((weight + 50) / 100, 1, 9) * 100);
		style._italic = face.IsItalic();
		return style;
	}

	/// @brief Loaded to read their face: only the ones of the folder, when asked
	/// @param fontSettings
	/// @param faceAsset
	/// @param message
	/// @return
	uint32_t FontHelper::DetectStyles(FontSettings& fontSettings, const std::shared_ptr<Asset>& faceAsset, String& message)
	{
		std::shared_ptr<FontFaceResource> faceResource = faceAsset != nullptr ? ResourceManager::GetInstance()->GetResource<FontFaceResource>(faceAsset->GetUid()) : nullptr;
		if (faceResource == nullptr || faceResource->GetFontFace().IsLoaded() == false)
		{
			message = "The face is not loaded";
			return 0;
		}

		Vector<FontSettings::Style> found;
		String                      skipped;
		auto                        add = [&fontSettings, &found, &skipped](const Asset& candidateAsset, const FontFace& face)
		{
			for (const FontSettings::Style& style : fontSettings._styles)
			{
				if (style._face.GetUid() == candidateAsset.GetUid())
				{
					return; // already in
				}
			}

			FontSettings::Style candidate = DetectStyle(face);
			bool                taken = false;
			for (const FontSettings::Style& style : fontSettings._styles)
			{
				taken |= style._face.GetUid().IsValid() && style._weight == candidate._weight && style._italic == candidate._italic;
			}
			for (const FontSettings::Style& style : found)
			{
				taken |= style._weight == candidate._weight && style._italic == candidate._italic;
			}
			if (taken)
			{
				skipped += skipped.Empty() ? candidateAsset.GetName() : String(", ") + candidateAsset.GetName();
				return;
			}

			candidate._face.SetUid(candidateAsset.GetUid());
			found.PushBack(candidate);
		};

		add(*faceAsset, faceResource->GetFontFace()); // first: the one kept when another one says the same style

		// The family of the face in its folder
		const String family = faceResource->GetFontFace().GetFamilyName();
		const float  width = faceResource->GetFontFace().GetWidth();
		AssetDatabase::FileSystemMapping* node = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(faceAsset->GetPath());
		if (family.Empty() == false && node != nullptr && node->_parentFolder != nullptr)
		{
			for (const AssetDatabase::FileSystemMapping* sibling : node->_parentFolder->_childrenAsset)
			{
				const std::shared_ptr<Asset>& siblingAsset = sibling->_asset;
				if (siblingAsset == nullptr || siblingAsset->GetType() != faceAsset->GetType() || siblingAsset->GetUid() == faceAsset->GetUid())
				{
					continue;
				}

				std::shared_ptr<FontFaceResource> siblingFace = ResourceManager::GetInstance()->GetResource<FontFaceResource>(siblingAsset->GetUid());
				if (siblingFace != nullptr && siblingFace->GetFontFace().IsLoaded() && siblingFace->GetFontFace().GetFamilyName() == family &&
				    siblingFace->GetFontFace().GetWidth() == width)
				{
					add(*siblingAsset, siblingFace->GetFontFace());
				}
			}
		}

		std::sort(found.begin(), found.end(),
		          [](const FontSettings::Style& left, const FontSettings::Style& right)
		          {
			          return left._italic != right._italic ? right._italic : std::to_underlying(left._weight) < std::to_underlying(right._weight);
		          });

		// After the ones there
		for (const FontSettings::Style& style : found)
		{
			fontSettings._styles.PushBack(style);
		}

		message = found.Empty() ? String::Format("Nothing new of the family \"%s\" in its folder", family.CStr()) : String::Format("%u added", found.Size());
		if (skipped.Empty() == false)
		{
			message += String::Format(". Left out, their style is taken: %s", skipped.CStr());
		}
		return found.Size();
	}

	/// @brief
	/// @param faceAssets
	/// @param directory
	/// @return
	Path FontHelper::CreateFont(const Vector<std::shared_ptr<Asset>>& faceAssets, const Path& directory)
	{
		if (faceAssets.Empty() || faceAssets[0] == nullptr)
		{
			return Path();
		}

		FontSettings fontSettings;
		String       message;
		for (const std::shared_ptr<Asset>& faceAsset : faceAssets)
		{
			DetectStyles(fontSettings, faceAsset, message);
		}

		String                            name = faceAssets[0]->GetName();
		std::shared_ptr<FontFaceResource> firstFace = ResourceManager::GetInstance()->GetResource<FontFaceResource>(faceAssets[0]->GetUid());
		if (firstFace != nullptr && firstFace->GetFontFace().GetFamilyName().Empty() == false)
		{
			name = firstFace->GetFontFace().GetFamilyName();
		}

		Document settingsDocument;
		if (Serializer::Serialize(fontSettings, settingsDocument.GetRootNode()) == false)
		{
			return Path();
		}

		AssetContainer assetContainer;
		assetContainer.SetAssetType(Hash::ComputeXxh3_64("Font"));
		assetContainer.SetUid(UID::GenerateUID());

		Stream&            settingsStream = assetContainer.AddDataBlock("Settings", false);
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(settingsDocument, settingsStream) == false)
		{
			return Path();
		}

		Vector<uint8_t> thumbnail;
		FilterMode      filterMode = FilterMode::Linear;
		if (GenerateThumbnail(fontSettings, thumbnail, filterMode))
		{
			assetContainer.SetThumbnail(std::move(thumbnail), filterMode);
		}

		const Path assetPath = AssetDatabase::GenerateUniqueAssetPath(directory / (name + ".font"));
		if (assetContainer.Save(assetPath) == false)
		{
			return Path();
		}
		return assetPath;
	}

	/// @brief Each font is loaded to read its styles: only when asked
	/// @param face
	/// @return
	Vector<std::shared_ptr<Asset>> FontHelper::FindFontsUsing(const UID& face)
	{
		Vector<std::shared_ptr<Asset>> fonts;

		AssetDatabase*                            assetDatabase = AssetDatabase::GetInstance();
		Vector<AssetDatabase::FileSystemMapping*> fontNodes;
		assetDatabase->ListAsset(fontNodes, assetDatabase->GetAssetRootNode(), FontResource::GetReflectionDescriptor());
		for (const AssetDatabase::FileSystemMapping* fontNode : fontNodes)
		{
			std::shared_ptr<FontResource> font = ResourceManager::GetInstance()->GetResource<FontResource>(fontNode->_asset->GetUid());
			if (font == nullptr)
			{
				continue;
			}
			for (const FontResource::Style& style : font->GetStyles())
			{
				if (style._face == face)
				{
					fonts.PushBack(fontNode->_asset);
					break;
				}
			}
		}
		return fonts;
	}

	/// @brief Its faces for Regular and Bold, as the closest of its styles are: a line each, one when it is the same face. Sharp pixels for pixel art
	/// @param fontSettings
	/// @param thumbnail
	/// @param filterMode
	/// @return
	bool FontHelper::GenerateThumbnail(const FontSettings& fontSettings, Vector<uint8_t>& thumbnail, FilterMode& filterMode)
	{
		FontResource font;
		font.SetStyles(fontSettings.GetStyles());

		Vector<std::shared_ptr<FontFaceResource>> faces;
		for (FontResource::Weight weight : {FontResource::Weight::Regular, FontResource::Weight::Bold})
		{
			const UID faceUid = font.FindStyle(weight, false);
			if (faceUid.IsValid() == false)
			{
				continue;
			}
			std::shared_ptr<FontFaceResource> face = ResourceManager::GetInstance()->GetResource<FontFaceResource>(faceUid);
			if (face != nullptr && face->GetFontFace().IsLoaded() && (faces.Empty() || faces[0] != face))
			{
				faces.PushBack(face);
			}
		}

		Vector<FontThumbnail::Line> lines;
		bool                        pixelArt = false;
		for (const std::shared_ptr<FontFaceResource>& face : faces)
		{
			FontThumbnail::Line& line = lines.EmplaceBack();
			line._face = &face->GetFontFace();
			line._renderMode = face->GetRenderMode();
			line._pixelSize = face->GetPixelSize();
			pixelArt |= face->GetGlyphMode() == GlyphCache::Mode::Pixel;
		}

		Vector<uint8_t> thumbnailPixels;
		if (FontThumbnail::Generate(lines, thumbnailPixels) == false || Thumbnail::Encode(thumbnailPixels.Data(), FontThumbnail::Size, FontThumbnail::Size, thumbnail) == false)
		{
			return false;
		}
		filterMode = pixelArt ? FilterMode::Nearest : FilterMode::Linear;
		return true;
	}
}
