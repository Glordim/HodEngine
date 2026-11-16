#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTab.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTableWindow.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Game/DataStruct/DataStruct.hpp>
#include <HodEngine/Game/DataStruct/DataStructFactory.hpp>

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
					_readable = _data.Read(settingsDocument.GetRootNode());
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
	DataTableAssetData& DataTableEditorTab::GetData()
	{
		return _data;
	}

	/// @brief
	/// @return
	bool DataTableEditorTab::IsReadable() const
	{
		return _readable;
	}

	/// @brief
	/// @param key
	/// @return
	DataStruct* DataTableEditorTab::FindRow(const String& key) const
	{
		for (const DataTableAssetData::Row& row : _data._rows)
		{
			if (row._key == key)
			{
				return row._data;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param key
	/// @return
	DataStruct* DataTableEditorTab::AddRow(const String& key)
	{
		if (_readable == false || FindRow(key) != nullptr)
		{
			return nullptr;
		}

		const ReflectionDescriptor* rowDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(_data._rowType);
		if (rowDescriptor == nullptr)
		{
			return nullptr;
		}

		DataStruct* data = static_cast<DataStruct*>(rowDescriptor->CreateInstance());
		_data._rows.PushBack(DataTableAssetData::Row {key, data});
		return data;
	}

	/// @brief
	/// @param key
	void DataTableEditorTab::RemoveRow(const String& key)
	{
		for (uint32_t index = 0; index < _data._rows.Size(); ++index)
		{
			if (_data._rows[index]._key == key)
			{
				DataStruct* data = _data._rows[index]._data;
				data->GetReflectionDescriptorV().DeleteInstance(data);
				_data._rows.Erase(index);
				if (_rowSelectionKey == key)
				{
					_rowSelectionKey = "";
				}
				return;
			}
		}
	}

	/// @brief
	/// @param key
	/// @param newKey
	/// @return
	bool DataTableEditorTab::RenameRow(const String& key, const String& newKey)
	{
		if (newKey.Empty())
		{
			OUTPUT_ERROR("Data table: a row's key can't be empty");
			return false;
		}
		if (FindRow(newKey) != nullptr)
		{
			OUTPUT_ERROR("Data table: the key '{}' is taken by another row", newKey);
			return false;
		}

		const String oldKey = key; // key may be the row's own: changed below
		for (DataTableAssetData::Row& row : _data._rows)
		{
			if (row._key == oldKey)
			{
				row._key = newKey;
				if (_rowSelectionKey == oldKey)
				{
					_rowSelectionKey = newKey;
				}
				return true;
			}
		}
		return false;
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
		return FindRow(_rowSelectionKey);
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
		if (_readable == false)
		{
			return false; // Not read: its asset kept as it is
		}

		Document settingsDocument;
		if (_data.Write(settingsDocument.GetRootNode()) == false)
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
	/// @param document
	/// @return false when its asset couldn't be read: no undo
	bool DataTableEditorTab::CaptureUndoState(Document& document)
	{
		return _readable && _data.Write(document.GetRootNode());
	}

	/// @brief Its rows replaced (what the windows show is read from them each frame)
	/// @param document
	/// @return
	bool DataTableEditorTab::RestoreUndoState(const Document& document)
	{
		_readable = _data.Read(document.GetRootNode());
		return _readable;
	}

	/// @brief
	void DataTableEditorTab::DrawMenuBar()
	{
	}
}
