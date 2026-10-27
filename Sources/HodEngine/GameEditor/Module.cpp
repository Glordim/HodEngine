#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"

#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/SpriteRendererComponent.hpp"
#include "HodEngine/Game/Components/TextureRendererComponent.hpp"
#include "HodEngine/Game/Components/TextRendererComponent.hpp"
#include "HodEngine/Game/Components/Light/PointLightComponent.hpp"
#include "HodEngine/Game/Components/Light/DirLightComponent.hpp"
#include "HodEngine/Game/Components/Light/SpotLightComponent.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"

#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/MultiShapeCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/BoxCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/CircleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/CapsuleCollider2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/EdgeCollider2dComponent.hpp"

#include "HodEngine/Game/Components/Primitive/BoxComponent.hpp"

#include "HodEngine/GameEditor/CustomPropertyDrawer/ZOrderCustomPropertyDrawer.hpp"
#include "HodEngine/GameEditor/CustomPropertyDrawer/WeakComponentCustomPropertyDrawer.hpp"

#include "HodEngine/GameEditor/CustomComponentDrawer/Node2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CameraCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/BoxCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CircleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/CapsuleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/EdgeCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/MultiShapeCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/TextureRendererCustomComponentDrawer.hpp"

#include "HodEngine/GameEditor/DataItemEditor/DataItemCooker.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemEditorTab.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableCooker.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableEditorTab.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabCooker.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabEditorTab.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneCooker.hpp"
#include "HodEngine/GameEditor/SceneEditor/SceneEditorTab.hpp"

#include "HodEngine/Game/DataStruct/DataItemContainer.hpp"
#include "HodEngine/Game/DataStruct/DataItemResource.hpp"
#include "HodEngine/Game/DataStruct/DataStruct.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"
#include "HodEngine/Game/DataStruct/DataTableContainer.hpp"
#include "HodEngine/Game/DataStruct/DataTableResource.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/SceneResource.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Hash.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::game;
using namespace hod::editor;

namespace
{
	EditorModuleRegistration registration;
	EditorModuleRegistration dataStructRegistration; // The "Create/DataItem|DataTable/<data struct>" actions: from the project modules, registered again when they reload

	/// @brief "Create/<label>": an empty asset of this type
	void RegisterCreateEmptyAsset(const char* label, const char* assetType, const char* fileName)
	{
		// Captured: the strings are literals
		registration.RegisterContextualMenuAction({
			.path = String("Create/") + label,
			.group = "New",
			.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
			.execute =
				[assetType, fileName](const AssetBrowserWindow::Context& context)
				{
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / fileName);

					AssetContainer assetContainer;
					assetContainer.SetAssetType(Hash::ComputeXxh3_64(assetType));
					assetContainer.SetUid(UID::GenerateUID());
					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
	}

	/// @brief "Set as startup scene" of the project
	void RegisterSetAsStartupScene()
	{
		registration.RegisterContextualMenuAction({
			.path = "Set as startup scene",
			.group = "",
			.available =
				+[](const AssetBrowserWindow::Context& context)
				{
					if (context.selectedItems.Size() == 1)
					{
						AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
						if (asset != nullptr && asset->_asset != nullptr)
						{
							return asset->_asset->GetType() == Hash::ComputeXxh3_64("Scene");
						}
					}
					return false;
				},
			.execute =
				+[](const AssetBrowserWindow::Context& context)
				{
					AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
					if (asset != nullptr && asset->_asset != nullptr)
					{
						Project::GetInstance()->SetStartupScene(asset->_asset);
					}
				},
		});
	}

	/// @brief Saves the settings of a new DataItem/DataTable asset, of the data struct type
	template<typename _Container_>
	void SaveDataStructAsset(_Container_& container, const char* assetType, const ReflectionDescriptor& descriptor, const Path& assetPath)
	{
		Document settingsDocument;
		Serializer::Serialize(container, settingsDocument.GetRootNode());

		AssetContainer assetContainer;
		assetContainer.SetAssetType(Hash::ComputeXxh3_64(assetType));
		assetContainer.SetSubType(descriptor.GetType());
		assetContainer.SetUid(UID::GenerateUID());

		Stream&            settingsStream = assetContainer.AddDataBlock("Settings", false);
		DocumentWriterJson documentWriter;
		documentWriter.Write(settingsDocument, settingsStream);

		assetContainer.Save(assetPath);
	}

	/// @brief "Create/DataItem/<data struct>" and "Create/DataTable/<data struct>": the data structs come from the project modules
	void RegisterCreateDataStructActions()
	{
		dataStructRegistration.UnregisterAll();

		for (const auto& pair : DataStructFactory::GetInstance()->GetAllDescriptors())
		{
			ReflectionDescriptor* descriptor = pair.second;
			dataStructRegistration.RegisterContextualMenuAction({
				.path = String("Create/DataItem/") + descriptor->GetDisplayName(),
				.group = "New",
				.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
				.execute =
					[descriptor](const AssetBrowserWindow::Context& context)
					{
						DataItemContainer dataItemContainer(descriptor->CreateInstance<DataStruct>());
						Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / (descriptor->GetDisplayName() + ".dataitem"));
						SaveDataStructAsset(dataItemContainer, "DataItem", *descriptor, assetPath);
						context.assetBrowserWindow.Rename(assetPath);
					},
			});
		}
		for (const auto& pair : DataStructFactory::GetInstance()->GetAllDescriptors())
		{
			ReflectionDescriptor* descriptor = pair.second;
			dataStructRegistration.RegisterContextualMenuAction({
				.path = String("Create/DataTable/") + descriptor->GetDisplayName(),
				.group = "New",
				.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
				.execute =
					[descriptor](const AssetBrowserWindow::Context& context)
					{
						DataTableContainer dataTableContainer(descriptor->GetType());
						Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / (descriptor->GetDisplayName() + ".datatable"));
						SaveDataStructAsset(dataTableContainer, "DataTable", *descriptor, assetPath);
						context.assetBrowserWindow.Rename(assetPath);
					},
			});
		}
	}

	Event<>::Slot onProjectModulesReloadedSlot(&RegisterCreateDataStructActions);
}

HOD_STARTUP_MODULE(GameEditor)
{
	registration.RegisterCustomPropertyDrawer<ZOrder, ZOrderCustomPropertyDrawer>();
	registration.RegisterCustomPropertyDrawer<WeakComponentBase, WeakComponentCustomPropertyDrawer>();

	registration.RegisterCustomComponentDrawer<Node2dComponent, Node2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CameraComponent, CameraCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<TextureRendererComponent, TextureRendererCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<BoxCollider2dComponent, BoxCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CircleCollider2dComponent, CircleCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CapsuleCollider2dComponent, CapsuleCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<EdgeCollider2dComponent, EdgeCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<MultiShapeCollider2dComponent, MultiShapeCollider2dCustomComponentDrawer>();

	registration.RegisterCooker<SceneCooker>("Scene");
	registration.RegisterResource<SceneResource>("Scene");
	registration.RegisterEditorTab<SceneEditorTab>("Scene");

	registration.RegisterCooker<PrefabCooker>("Prefab");
	registration.RegisterResource<PrefabResource>("Prefab");
	registration.RegisterEditorTab<PrefabEditorTab>("Prefab");

	registration.RegisterCooker<DataItemCooker>("DataItem");
	registration.RegisterResource<DataItemResource>("DataItem");
	registration.RegisterEditorTab<DataItemEditorTab>("DataItem");

	registration.RegisterCooker<DataTableCooker>("DataTable");
	registration.RegisterResource<DataTableResource>("DataTable");
	registration.RegisterEditorTab<DataTableEditorTab>("DataTable");

	RegisterCreateEmptyAsset("Scene", "Scene", "NewScene.scene");
	RegisterCreateEmptyAsset("Prefab", "Prefab", "NewPrefab.prefab");
	RegisterSetAsStartupScene();

	RegisterCreateDataStructActions();
	Project::GetInstance()->GetOnModulesReloadedEvent().Connect(onProjectModulesReloadedSlot);
	return 0;
}

HOD_SHUTDOWN_MODULE(GameEditor)
{
	onProjectModulesReloadedSlot.Disconnect();
	dataStructRegistration.UnregisterAll();
	registration.UnregisterAll();
	return 0;
}
