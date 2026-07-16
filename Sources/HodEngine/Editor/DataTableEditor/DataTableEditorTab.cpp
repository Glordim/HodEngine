#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorTab.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorTableWindow.hpp"
#include "HodEngine/Editor/DataTableEditor/DataTableEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::inline editor
{
	/// @brief
	DataTableEditorTab::DataTableEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_TABLE)
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
					Serializer::Deserialize(_dataTableContainer, settingsDocument.GetRootNode());
				}
			}
		}
	}

	/// @brief
	DataTableEditorTab::~DataTableEditorTab()
	{
	}

	/// @brief
	/// @return
	DataTableContainer& DataTableEditorTab::GetDataTableContainer()
	{
		return _dataTableContainer;
	}

	/// @brief
	/// @param key
	void DataTableEditorTab::SetRowSelection(const String& key)
	{
		_rowSelectionKey = key;
	}

	/// @brief
	/// @return
	const String& DataTableEditorTab::GetRowSelectionKey() const
	{
		return _rowSelectionKey;
	}

	/// @brief
	/// @return
	DataStruct* DataTableEditorTab::GetRowSelection() const
	{
		return const_cast<DataTableContainer&>(_dataTableContainer).FindRow(_rowSelectionKey);
	}

	/// @brief
	void DataTableEditorTab::CreateDefaultLayout()
	{
		DataTableEditorTableWindow*      tableWindow = OpenWindow<DataTableEditorTableWindow>();
		DataTableEditorPropertiesWindow* propertiesWindow = OpenWindow<DataTableEditorPropertiesWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Right, 0.3f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(tableWindow->GetIdentifier(), opposite);
		ImGui::DockBuilderDockWindow(propertiesWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief
	/// @return
	bool DataTableEditorTab::DrawContent()
	{
		return true;
	}

	/// @brief
	/// @return
	bool DataTableEditorTab::OnSave()
	{
		Document settingsDocument;
		if (Serializer::Serialize(_dataTableContainer, settingsDocument.GetRootNode()) == false)
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
	void DataTableEditorTab::DrawMenuBar()
	{
	}
}
