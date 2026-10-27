#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/UI2Editor/CustomPropertyDrawer/RectOffsetsCustomPropertyDrawer.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabCooker.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"

#include <HodEngine/Core/Hash.hpp>
#include <HodEngine/UI2/Margin.hpp>
#include <HodEngine/UI2/Padding.hpp>
#include <HodEngine/UI2/UIPrefabResource.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	EditorModuleRegistration registration;
}

HOD_STARTUP_MODULE(UI2Editor)
{
	// The same cross for both, pointing the way each one pushes
	registration.RegisterCustomPropertyDrawer(ui2::Margin::GetReflectionDescriptor(),
	                                          DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Outward));
	registration.RegisterCustomPropertyDrawer(ui2::Padding::GetReflectionDescriptor(),
	                                          DefaultAllocator::GetInstance().New<RectOffsetsCustomPropertyDrawer>(RectOffsetsCustomPropertyDrawer::Direction::Inward));

	registration.RegisterCooker<UIPrefabCooker>("UIPrefab");
	registration.RegisterResource<UIPrefabResource>("UIPrefab");
	registration.RegisterEditorTab<UIPrefabEditorTab>("UIPrefab");

	registration.RegisterContextualMenuAction({
		.path = "Create/UI Prefab",
		.group = "New",
		.available = +[](const AssetBrowserWindow::Context& context) { return context.selectedItems.Empty(); },
		.execute =
			+[](const AssetBrowserWindow::Context& context)
			{
				Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / "NewUIPrefab.uiprefab");

				AssetContainer assetContainer;
				assetContainer.SetAssetType(Hash::ComputeXxh3_64("UIPrefab"));
				assetContainer.SetUid(UID::GenerateUID());
				assetContainer.Save(assetPath);

				context.assetBrowserWindow.Rename(assetPath);
			},
	});
	return 0;
}

HOD_SHUTDOWN_MODULE(UI2Editor)
{
	registration.UnregisterAll();
	return 0;
}
