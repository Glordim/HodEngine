#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemAssetType.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemCooker.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemEditorTab.hpp"

#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Game/DataStruct/DataItemContainer.hpp>
#include <HodEngine/Game/DataStruct/DataItemResource.hpp>
#include <HodEngine/Game/DataStruct/DataStruct.hpp>
#include <HodEngine/Game/DataStruct/DataStructFactory.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::inline editor
{
	/// @brief
	DataItemAssetType::DataItemAssetType()
	: _onProjectModulesReloadedSlot([this]() { RegisterCreateActions(); })
	{
	}

	/// @brief
	/// @return
	Cooker* DataItemAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<DataItemCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* DataItemAssetType::GetCookedResource() const
	{
		return &DataItemResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* DataItemAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<DataItemEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* DataItemAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetDataItemTexture();
	}

	/// @brief
	/// @param registration
	void DataItemAssetType::OnRegistered(EditorModuleRegistration& registration)
	{
		(void)registration; // Its own: registered again when the project modules reload
		RegisterCreateActions();
		Project::GetInstance()->GetOnModulesReloadedEvent().Connect(_onProjectModulesReloadedSlot);
	}

	/// @brief
	void DataItemAssetType::RegisterCreateActions()
	{
		_createActionsRegistration.UnregisterAll();

		for (const auto& pair : DataStructFactory::GetInstance()->GetAllDescriptors())
		{
			ReflectionDescriptor* descriptor = pair.second;
			_createActionsRegistration.RegisterContextualMenuAction({
				.path = String("Create/DataItem/") + descriptor->GetDisplayName(),
				.group = "New",
				.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
				.execute =
					[descriptor](const AssetBrowserWindow::Context& context)
					{
						DataItemContainer container(descriptor->CreateInstance<DataStruct>());
						Document settingsDocument;
						Serializer::Serialize(container, settingsDocument.GetRootNode());

						AssetContainer assetContainer;
						assetContainer.SetAssetType(GetHash());
						assetContainer.SetSubType(descriptor->GetType());
						assetContainer.SetUid(UID::GenerateUID());

						Stream&            settingsStream = assetContainer.AddDataBlock("Settings", false);
						DocumentWriterJson documentWriter;
						documentWriter.Write(settingsDocument, settingsStream);

						Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / (descriptor->GetDisplayName() + ".dataitem"));
						assetContainer.Save(assetPath);
						context.assetBrowserWindow.Rename(assetPath);
					},
			});
		}
	}

	/// @brief
	void DataItemAssetType::OnUnregistered()
	{
		_onProjectModulesReloadedSlot.Disconnect();
		_createActionsRegistration.UnregisterAll();
	}
}
