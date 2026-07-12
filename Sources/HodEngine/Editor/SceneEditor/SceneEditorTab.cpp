#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Stream/SpillStream.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/SceneEditor/SceneEditorTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Game/PrefabResource.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/World.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

namespace hod::inline editor
{
	/// @brief
	SceneEditorTab::SceneEditorTab(std::shared_ptr<Asset> asset)
	: EntityEditorTab(asset, ICON_MDI_IMAGE_FILTER_HDR)
	{
		if (asset != nullptr)
		{
			_scene->SetName(asset->GetName());

			const AssetContainer::DataBlockInfo* entitiesDataBlock = _assetContainer.FindDataBlock("Entities");
			if (entitiesDataBlock == nullptr)
			{
				return;
			}

			Document           document;
			DocumentReaderJson documentReader;
			if (documentReader.Read(document, *entitiesDataBlock->_stream) == false)
			{
				return; // todo message + bool
			}

			if (Serializer::Deserialize(_scene, document.GetRootNode()) == false)
			{
				return; // todo message + bool
			}

			_scene->ProcessActivation();
		}
	}

	/// @brief
	SceneEditorTab::~SceneEditorTab() {}

	/// @brief
	/// @return
	bool SceneEditorTab::OnSave()
	{
		std::shared_ptr<Asset> asset = GetAsset();
		if (asset == nullptr)
		{
			return false;
		}

		_assetContainer.ClearDataBlocks();

		SpillStream entitiesStream;

		Document document;
		if (Serializer::Serialize(_scene, document.GetRootNode()) == false)
		{
			return false; // todo message + bool
		}

		DocumentWriterJson documentwriter;
		if (documentwriter.Write(document, entitiesStream) == false)
		{
			return false; // todo message + bool
		}
		_assetContainer.SetDataBlock("Entities", entitiesStream, false);

		return true;
	}

	/// @brief
	void SceneEditorTab::CreateDefaultLayout()
	{
		// ImGuiManager::GetInstance()->OpenWindow<AssetBrowserWindow>();
		HierachyWindow*  hierarchyWindow = OpenWindow<HierachyWindow>();
		InspectorWindow* inspectorWindow = OpenWindow<InspectorWindow>();
		ViewportWindow*  viewportWindow = OpenWindow<ViewportWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &opposite);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Right, 0.2f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(hierarchyWindow->GetIdentifier(), dockLeft);
		ImGui::DockBuilderDockWindow(inspectorWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief
	/// @return
	bool SceneEditorTab::DrawContent()
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
}
