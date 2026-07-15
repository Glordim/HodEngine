#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DataItemEditor/DataItemEditorTab.hpp"
#include "HodEngine/Editor/DataItemEditor/DataItemEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

namespace hod::inline editor
{
	/// @brief
	DataItemEditorTab::DataItemEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_TEXT_BOX)
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
					Serializer::Deserialize(_dataItemContainer, settingsDocument.GetRootNode());
				}
			}
		}
	}

	/// @brief
	DataItemEditorTab::~DataItemEditorTab()
	{
	}

	/// @brief
	/// @return
	DataItemContainer& DataItemEditorTab::GetDataItemContainer()
	{
		return _dataItemContainer;
	}

	/// @brief
	void DataItemEditorTab::CreateDefaultLayout()
	{
		DataItemEditorPropertiesWindow* propertiesWindow = OpenWindow<DataItemEditorPropertiesWindow>();

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
	bool DataItemEditorTab::DrawContent()
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
	/// @return
	bool DataItemEditorTab::OnSave()
	{
		Document settingsDocument;
		if (Serializer::Serialize(_dataItemContainer, settingsDocument.GetRootNode()) == false)
		{
			return false;
		}

		Stream& settingsStream = _assetContainer.AddDataBlock("Settings", false);
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(settingsDocument, settingsStream) == false)
		{
			return false;
		}

		return true;
	}

	/// @brief
	void DataItemEditorTab::DrawMenuBar()
	{
	}
}
