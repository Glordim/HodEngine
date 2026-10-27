#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontEditorTab.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontEditorPropertiesWindow.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontHelper.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontPreviewGlyphsWindow.hpp"
#include "HodEngine/RendererEditor/FontEditor/FontPreviewViewportWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/EnumDescriptor.hpp>
#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Renderer/Text/FontChain.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param asset
	FontEditorTab::FontEditorTab(std::shared_ptr<Asset> asset)
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
					Serializer::Deserialize(_fontSettings, settingsDocument.GetRootNode());
				}
			}

			_font = ResourceManager::GetInstance()->GetResource<FontResource>(asset->GetUid());
			ApplySettings();

			_savedSettings = _fontSettings;
		}
	}

	/// @brief Reverts the resource, shared with whatever uses it, to what is saved: the live edits are not
	FontEditorTab::~FontEditorTab()
	{
		if (_font != nullptr && IsDirty())
		{
			_fontSettings = _savedSettings;
			ApplySettings();
		}
	}

	/// @brief
	/// @return
	std::shared_ptr<FontResource> FontEditorTab::GetFont() const
	{
		return _font;
	}

	/// @brief
	/// @return
	FontSettings& FontEditorTab::GetFontSettings()
	{
		return _fontSettings;
	}

	/// @brief The text drawn with the font follows: TextNode reads its font each time it lays its text out
	void FontEditorTab::ApplySettings()
	{
		if (_font != nullptr)
		{
			_font->SetStyles(_fontSettings.GetStyles());
			_font->SetFallbacks(_fontSettings.GetFallbacks(_font->GetUid()));
		}
		InvalidatePreview();
	}

	/// @brief
	/// @param fontChain
	/// @return
	bool FontEditorTab::UpdateFontChain(FontChain& fontChain) const
	{
		return fontChain.Update(_font, _previewWeight, _previewItalic);
	}

	/// @brief
	void FontEditorTab::CreateDefaultLayout()
	{
		FontEditorPropertiesWindow* propertiesWindow = OpenWindow<FontEditorPropertiesWindow>();
		FontPreviewViewportWindow*  viewportWindow = OpenWindow<FontPreviewViewportWindow>();
		FontPreviewGlyphsWindow*    glyphsWindow = OpenWindow<FontPreviewGlyphsWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Right, 0.3f, nullptr, &opposite);
		ImGuiID dockTop = opposite;
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Down, 0.35f, nullptr, &dockTop);

		// The preview, the glyphs below it
		ImGui::DockBuilderDockWindow(propertiesWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), dockTop);
		ImGui::DockBuilderDockWindow(glyphsWindow->GetIdentifier(), dockBottom);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief
	/// @return
	bool FontEditorTab::DrawContent()
	{
		return true;
	}

	/// @brief The style the preview is drawn in
	void FontEditorTab::DrawMenuBarOptions()
	{
		const EnumDescriptor& enumDescriptor = ReflectedEnum<FontResource::Weight>::GetEnumDescriptor();
		const String          weightName(enumDescriptor.ToString(_previewWeight));
		const float           itemTop = ImGui::GetCursorScreenPos().y; // centered on the bar by DrawMenuBar: SameLine goes back to the top of the line

		ImGui::SetNextItemWidth(140.0f);
		if (ImGui::BeginCombo("##PreviewWeight", weightName.CStr()))
		{
			for (uint16_t value = 100; value <= 900; value += 100)
			{
				const FontResource::Weight weight = static_cast<FontResource::Weight>(value);
				const String               label = String::Format("%s (%u)", String(enumDescriptor.ToString(weight)).CStr(), (uint32_t)value);
				if (ImGui::Selectable(label.CStr(), weight == _previewWeight) && weight != _previewWeight)
				{
					_previewWeight = weight;
					InvalidatePreview();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("The weight the preview asks for: drawn with the closest face");
		}
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, itemTop));
		if (ImGui::Checkbox("Italic##Preview", &_previewItalic))
		{
			InvalidatePreview();
		}
		ImGui::SameLine(0.0f, 8.0f);
	}

	/// @brief
	/// @return
	bool FontEditorTab::OnSave()
	{
		Document settingsDocument;
		if (Serializer::Serialize(_fontSettings, settingsDocument.GetRootNode()) == false)
		{
			return false;
		}

		Stream&            settingsStream = _assetContainer.AddDataBlock("Settings", false);
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(settingsDocument, settingsStream) == false)
		{
			return false;
		}

		Vector<uint8_t> thumbnail;
		FilterMode      filterMode = FilterMode::Linear;
		if (FontHelper::GenerateThumbnail(_fontSettings, thumbnail, filterMode))
		{
			_assetContainer.SetThumbnail(std::move(thumbnail), filterMode);
		}
		else
		{
			_assetContainer.ClearThumbnail(); // no face: the icon of the fonts
		}

		_savedSettings = _fontSettings;
		return true;
	}
}
