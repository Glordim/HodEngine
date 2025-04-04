#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/SerializedDataEditor/SerializedDataEditorTab.hpp"
#include "HodEngine/Editor/SerializedDataEditor/SerializedDataEditorPropertiesWindow.hpp"

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
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"

namespace hod::editor
{
	/// @brief 
	SerializedDataEditorTab::SerializedDataEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_TABLE)
	{
		if (asset != nullptr)
		{
			Document document;
			DocumentReaderJson reader;
			reader.Read(document, asset->GetPath());

			Serializer::Deserialize(_serializedDataAsset, document.GetRootNode());
		}
	}

	/// @brief 
	SerializedDataEditorTab::~SerializedDataEditorTab()
	{
	}

	/// @brief 
	/// @return 
	SerializedDataAsset& SerializedDataEditorTab::GetSerializedDataAsset()
	{
		return _serializedDataAsset;
	}

	/// @brief 
	void SerializedDataEditorTab::CreateDefaultLayout()
	{
		SerializedDataEditorPropertiesWindow* propertiesWindow = OpenWindow<SerializedDataEditorPropertiesWindow>();

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
	bool SerializedDataEditorTab::DrawContent()
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
	void SerializedDataEditorTab::DrawMenuBar()
	{
	}
}
