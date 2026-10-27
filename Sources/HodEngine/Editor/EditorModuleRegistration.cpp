#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"

namespace hod::inline editor
{
	/// @brief
	EditorModuleRegistration::~EditorModuleRegistration()
	{
		UnregisterAll();
	}

	/// @brief
	/// @param contextualMenuAction
	void EditorModuleRegistration::RegisterContextualMenuAction(AssetBrowserWindow::ContextualMenuAction contextualMenuAction)
	{
		_contextualMenuActionIds.PushBack(AssetBrowserWindow::RegisterContextualMenuAction(std::move(contextualMenuAction)));
	}

	/// @brief
	void EditorModuleRegistration::UnregisterAll()
	{
		for (uint32_t contextualMenuActionId : _contextualMenuActionIds)
		{
			AssetBrowserWindow::UnregisterContextualMenuAction(contextualMenuActionId);
		}
		_contextualMenuActionIds.Clear();

		// The tabs first: closing them can still use the asset types
		for (const String& assetType : _editorTabAssetTypes)
		{
			Editor::GetInstance()->UnregisterEditorTab(assetType);
		}
		_editorTabAssetTypes.Clear();

		for (const String& assetType : _resourceAssetTypes)
		{
			AssetDatabase::GetInstance()->UnregisterResource(assetType);
		}
		_resourceAssetTypes.Clear();

		for (const String& assetType : _cookerAssetTypes)
		{
			AssetDatabase::GetInstance()->UnregisterCooker(assetType);
		}
		_cookerAssetTypes.Clear();

		for (void (*importerUnregister)() : _importerUnregisters)
		{
			importerUnregister();
		}
		_importerUnregisters.Clear();
	}
}
