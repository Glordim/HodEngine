#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableAssetType.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableCooker.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTab.hpp"

#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Game/DataStruct/DataStructFactory.hpp>
#include <HodEngine/Game/DataStruct/DataTableContainer.hpp>
#include <HodEngine/Game/DataStruct/DataTableResource.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::inline editor
{
	/// @brief
	DataTableAssetType::DataTableAssetType()
	: _onProjectModulesReloadedSlot([this]() { RegisterCreateActions(); })
	{
	}

	/// @brief
	/// @return
	Cooker* DataTableAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<DataTableCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* DataTableAssetType::GetCookedResource() const
	{
		return &DataTableResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* DataTableAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<DataTableEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* DataTableAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetDataTableTexture();
	}

	/// @brief
	/// @param editorRegistrar
	void DataTableAssetType::OnRegistered(EditorRegistrar& editorRegistrar)
	{
		(void)editorRegistrar; // Its own: registered again when the project modules reload
		RegisterCreateActions();
		Project::GetInstance()->GetOnModulesReloadedEvent().Connect(_onProjectModulesReloadedSlot);
	}

	/// @brief
	void DataTableAssetType::RegisterCreateActions()
	{
		_createActionsRegistration.UnregisterAll();

		EditorRegistrar editorRegistrar(_createActionsRegistration);
		for (const auto& pair : DataStructFactory::GetInstance()->GetAllDescriptors())
		{
			ReflectionDescriptor* descriptor = pair.second;
			editorRegistrar.RegisterContextualMenuAction({
				.path = String("Create/DataTable/") + descriptor->GetDisplayName(),
				.group = "New",
				.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
				.execute =
					[descriptor](const AssetBrowserWindow::Context& context)
					{
						DataTableContainer container(descriptor->GetType());
						Document settingsDocument;
						Serializer::Serialize(container, settingsDocument.GetRootNode());

						AssetContainer assetContainer;
						assetContainer.SetAssetType(GetHash());
						assetContainer.SetSubType(descriptor->GetType());
						assetContainer.SetUid(UID::GenerateUID());

						Stream&            settingsStream = assetContainer.AddDataBlock("Settings", false);
						DocumentWriterJson documentWriter;
						documentWriter.Write(settingsDocument, settingsStream);

						Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / GetFileName(descriptor->GetDisplayName()));
						assetContainer.Save(assetPath);
						context.assetBrowserWindow.Rename(assetPath);
					},
			});
		}
	}

	/// @brief
	void DataTableAssetType::OnUnregistered()
	{
		_onProjectModulesReloadedSlot.Disconnect();
		_createActionsRegistration.UnregisterAll();
	}
}
