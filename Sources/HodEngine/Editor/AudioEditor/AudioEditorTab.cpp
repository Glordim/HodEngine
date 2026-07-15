#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AudioEditor/AudioEditorTab.hpp"
#include "HodEngine/Editor/AudioEditor/AudioEditorPropertiesWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

namespace hod::inline editor
{
	/// @brief 
	AudioEditorTab::AudioEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_VOLUME_HIGH)
	{
	}

	/// @brief 
	AudioEditorTab::~AudioEditorTab()
	{
	}

	/// @brief 
	void AudioEditorTab::CreateDefaultLayout()
	{
		AudioEditorPropertiesWindow* propertiesWindow = OpenWindow<AudioEditorPropertiesWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGui::DockBuilderDockWindow(propertiesWindow->GetIdentifier(), _dockSpaceId);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief 
	/// @return 
	bool AudioEditorTab::DrawContent()
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
	void AudioEditorTab::DrawMenuBar()
	{
	}
}
