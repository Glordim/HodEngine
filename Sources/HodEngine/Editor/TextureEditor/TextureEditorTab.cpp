#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorTab.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorPropertiesWindow.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorViewportWindow.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/Resource/ResourceManager.hpp>

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::editor
{
	/// @brief 
	TextureEditorTab::TextureEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_CHECKERBOARD)
	{
		if (asset != nullptr)
		{
			_texture = ResourceManager::GetInstance()->GetResource<renderer::TextureResource>(asset->GetMeta()._uid);
		}
	}

	/// @brief 
	TextureEditorTab::~TextureEditorTab()
	{
	}

	/// @brief 
	/// @return 
	std::shared_ptr<renderer::TextureResource> TextureEditorTab::GetTexture() const
	{
		return _texture;
	}

	/// @brief 
	/// @return 
	float TextureEditorTab::GetZoomFactor() const
	{
		return _zoomFactor;
	}

	/// @brief 
	void TextureEditorTab::SetZoomFactor(float zoomFactor)
	{
		_zoomFactor = std::max(0.01f, zoomFactor);
	}

	/// @brief 
	void TextureEditorTab::CreateDefaultLayout()
	{
		TextureEditorPropertiesWindow* propertiesWindow = OpenWindow<TextureEditorPropertiesWindow>();
		TextureEditorViewportWindow* viewportWindow = OpenWindow<TextureEditorViewportWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(propertiesWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief 
	/// @return 
	bool TextureEditorTab::DrawContent()
	{
		// todo override GetIdentifier ?
		/*
		if (_asset != nullptr && _assetWasDirty == true && _asset->IsDirty() == false)
		{
			_assetWasDirty = false;
			SetTitle(_asset->GetName());
		}
		*/
		//

		return true;
	}

	/// @brief 
	void TextureEditorTab::DrawMenuBar()
	{
		ImGui::SameLine(0.0f, 8.0f);

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		bool toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Red));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.8f, 0.0f, 0.0f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("R", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Red));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Red);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 4.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Green));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.0f, 0.8f, 0.0f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("G", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Green));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Green);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 4.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Blue));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.0f, 0.0f, 0.8f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(0.0f, 0.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(0.0f, 0.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("B", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Blue));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Blue);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 4.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Alpha));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.8f, 0.8f, 0.8f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("A", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Alpha));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Alpha);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 8.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		if (_maxMipmapLevel == 0)
		{
			ImGui::BeginDisabled();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("No Mipmap");
			ImGui::EndDisabled();
		}
		else
		{
			uint8_t minMipmapLevel = 0;
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Mipmap");
			ImGui::SameLine(0.0f, 8.0f);
			cursorPos = ImGui::GetCursorScreenPos();
			ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

			ImGui::SetNextItemWidth(160);
			ImGui::SliderScalar("##MipmapLevel", ImGuiDataType_U8, &_mipmapLevel, &minMipmapLevel, &_maxMipmapLevel);
		}

		ImGui::SameLine(0.0f, 8.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Zoom");
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		ImGui::SetNextItemWidth(160);
		ImGui::SliderFloat("##Zoom", &_zoomFactor, 0.01f, 2.0f, "");
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%u%%", static_cast<uint32_t>(_zoomFactor * 100.0f));

		ImGui::SameLine(0.0f, 8.0f);
		cursorPos.x += ImGui::CalcTextSize("100%").x;
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));
		ImGui::BeginDisabled(_zoomFactor == 1.0f);
		if (ImGui::Button(ICON_MDI_UNDO, ImVec2(0, 28)) == true)
		{
			_zoomFactor = 1.0f;
		}
		ImGui::EndDisabled();
	}
}
