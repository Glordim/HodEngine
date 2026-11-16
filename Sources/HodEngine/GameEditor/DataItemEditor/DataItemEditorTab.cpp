#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemEditorTab.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Game/DataStruct/DataStructFactory.hpp>
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
					_data = Serializer::DeserializeWithType<DataStruct>(settingsDocument.GetRootNode(),
					                                                    [](RttiType type) { return DataStructFactory::GetInstance()->FindReflectionDescriptor(type); });
					if (_data == nullptr)
					{
						OUTPUT_ERROR("Data item {}: unable to read its data (data struct {}, unknown when its module isn't loaded)", asset->GetName(),
						             Serializer::ReadType(settingsDocument.GetRootNode()));
					}
				}
			}
		}
	}

	/// @brief
	DataItemEditorTab::~DataItemEditorTab()
	{
		if (_data != nullptr)
		{
			_data->GetReflectionDescriptorV().DeleteInstance(_data);
		}
	}

	/// @brief
	/// @return
	DataStruct* DataItemEditorTab::GetDataItem()
	{
		return _data;
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
		if (_data == nullptr)
		{
			return false; // Not read: its asset kept as it is
		}

		Document settingsDocument;
		if (Serializer::SerializeWithType(*_data, settingsDocument.GetRootNode()) == false)
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
