#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceEditorTab.hpp"
#include "HodEngine/Editor/FontEditor/FontHelper.hpp"
#include "HodEngine/Editor/FontEditor/FontPreviewGlyphsWindow.hpp"
#include "HodEngine/Editor/FontEditor/FontPreviewViewportWindow.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceEditorAtlasWindow.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceEditorPropertiesWindow.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontThumbnail.hpp"
#include "HodEngine/Editor/Thumbnail.hpp"

#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>
#include <HodEngine/Renderer/Text/FontChain.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param asset
	FontFaceEditorTab::FontFaceEditorTab(std::shared_ptr<Asset> asset)
	: FontPreviewTab(asset, ICON_MDI_FORMAT_FONT)
	{
		if (asset != nullptr)
		{
			const AssetContainer::DataBlockInfo* settingsDataBlock = _assetContainer.FindDataBlock("Settings");
			if (settingsDataBlock != nullptr)
			{
				Document           settingsDocument;
				DocumentReaderJson documentReader;
				if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == true)
				{
					Serializer::Deserialize(_fontFaceSettings, settingsDocument.GetRootNode());
				}
			}

			_fontFace = ResourceManager::GetInstance()->GetResource<FontFaceResource>(asset->GetUid());
			ApplySettings();

			_savedSettings = _fontFaceSettings;
			FindFonts();
		}
	}

	/// @brief Reverts the resource, shared with whatever uses it, to what is saved: the live edits are not
	FontFaceEditorTab::~FontFaceEditorTab()
	{
		if (_fontFace != nullptr && IsDirty())
		{
			_fontFaceSettings = _savedSettings;
			ApplySettings();
		}
	}

	/// @brief
	/// @return
	std::shared_ptr<FontFaceResource> FontFaceEditorTab::GetFontFace() const
	{
		return _fontFace;
	}

	/// @brief
	/// @return
	FontFaceSettings& FontFaceEditorTab::GetFontFaceSettings()
	{
		return _fontFaceSettings;
	}

	/// @brief The text drawn with the face follows: TextNode reads the settings of its faces each time it lays its text out
	void FontFaceEditorTab::ApplySettings()
	{
		if (_fontFace != nullptr)
		{
			_fontFace->SetRenderMode(_fontFaceSettings._renderMode);
			_fontFace->SetPixelSize(_fontFaceSettings._pixelSize);
		}
		InvalidatePreview();
	}

	/// @brief
	/// @param fontChain
	/// @return
	bool FontFaceEditorTab::UpdateFontChain(FontChain& fontChain) const
	{
		return fontChain.Update(_fontFace);
	}

	/// @brief
	/// @return
	const Vector<std::shared_ptr<Asset>>& FontFaceEditorTab::GetFonts() const
	{
		return _fonts;
	}

	/// @brief
	void FontFaceEditorTab::FindFonts()
	{
		_fonts = FontHelper::FindFontsUsing(GetAsset()->GetUid());
	}

	/// @brief
	void FontFaceEditorTab::CreateDefaultLayout()
	{
		FontFaceEditorPropertiesWindow* propertiesWindow = OpenWindow<FontFaceEditorPropertiesWindow>();
		FontPreviewViewportWindow*      viewportWindow = OpenWindow<FontPreviewViewportWindow>();
		FontPreviewGlyphsWindow*        glyphsWindow = OpenWindow<FontPreviewGlyphsWindow>();
		FontFaceEditorAtlasWindow*      atlasWindow = OpenWindow<FontFaceEditorAtlasWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Right, 0.25f, nullptr, &opposite);
		ImGuiID dockTop = opposite;
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Down, 0.35f, nullptr, &dockTop);
		ImGuiID dockTopLeft = dockTop;
		ImGuiID dockTopRight = ImGui::DockBuilderSplitNode(dockTop, ImGuiDir_Right, 0.4f, nullptr, &dockTopLeft);

		// The preview and the atlas side by side, the glyphs below them
		ImGui::DockBuilderDockWindow(propertiesWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), dockTopLeft);
		ImGui::DockBuilderDockWindow(atlasWindow->GetIdentifier(), dockTopRight);
		ImGui::DockBuilderDockWindow(glyphsWindow->GetIdentifier(), dockBottom);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief
	/// @return
	bool FontFaceEditorTab::DrawContent()
	{
		return true;
	}

	/// @brief
	/// @return
	bool FontFaceEditorTab::OnSave()
	{
		Document settingsDocument;
		if (Serializer::Serialize(_fontFaceSettings, settingsDocument.GetRootNode()) == false)
		{
			return false;
		}

		Stream&            settingsStream = _assetContainer.AddDataBlock("Settings", false);
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(settingsDocument, settingsStream) == false)
		{
			return false;
		}

		// As the face is drawn with its settings: sharp pixels for pixel art
		Vector<uint8_t> thumbnailPixels;
		Vector<uint8_t> thumbnail;
		if (_fontFace != nullptr && FontThumbnail::Generate(_fontFace->GetFontFace(), _fontFaceSettings._renderMode, _fontFaceSettings._pixelSize, thumbnailPixels) &&
		    Thumbnail::Encode(thumbnailPixels.Data(), FontThumbnail::Size, FontThumbnail::Size, thumbnail))
		{
			const bool pixelArt = _fontFaceSettings._renderMode == FontFaceResource::RenderMode::PixelArt && _fontFaceSettings._pixelSize != 0;
			_assetContainer.SetThumbnail(std::move(thumbnail), pixelArt ? FilterMode::Nearest : FilterMode::Linear);
		}

		_savedSettings = _fontFaceSettings;
		return true;
	}
}
