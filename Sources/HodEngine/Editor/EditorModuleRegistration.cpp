#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"
#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/Editor/WindowFactory.hpp"

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
	/// @param dropAction
	void EditorModuleRegistration::RegisterDropAction(AssetBrowserWindow::DropAction dropAction)
	{
		_dropActionIds.PushBack(AssetBrowserWindow::RegisterDropAction(std::move(dropAction)));
	}

	/// @brief
	/// @param descriptor
	/// @param drawer Owned by the registry
	void EditorModuleRegistration::RegisterCustomPropertyDrawer(const ReflectionDescriptor& descriptor, CustomPropertyDrawer* drawer)
	{
		CustomPropertyDrawerRegistry::Register(descriptor, drawer);
		_customPropertyDrawerDescriptors.PushBack(&descriptor);
	}

	/// @brief
	/// @param descriptor
	/// @param drawer Owned by the registry
	void EditorModuleRegistration::RegisterCustomComponentDrawer(const ReflectionDescriptor& descriptor, CustomComponentDrawer* drawer)
	{
		CustomComponentDrawerRegistry::Register(descriptor, drawer);
		_customComponentDrawerDescriptors.PushBack(&descriptor);
	}

	/// @brief
	/// @param route
	/// @param descriptor
	void EditorModuleRegistration::RegisterWindow(const String& route, ReflectionDescriptor& descriptor)
	{
		WindowFactory::GetInstance()->Register(route, descriptor);
		_windowDescriptors.PushBack(&descriptor);
	}

	/// @brief
	void EditorModuleRegistration::UnregisterAll()
	{
		for (ReflectionDescriptor* descriptor : _windowDescriptors)
		{
			WindowFactory::GetInstance()->Unregister(*descriptor);
		}
		_windowDescriptors.Clear();

		for (const ReflectionDescriptor* descriptor : _customComponentDrawerDescriptors)
		{
			CustomComponentDrawerRegistry::Unregister(*descriptor);
		}
		_customComponentDrawerDescriptors.Clear();

		for (const ReflectionDescriptor* descriptor : _customPropertyDrawerDescriptors)
		{
			CustomPropertyDrawerRegistry::Unregister(*descriptor);
		}
		_customPropertyDrawerDescriptors.Clear();

		for (uint32_t contextualMenuActionId : _contextualMenuActionIds)
		{
			AssetBrowserWindow::UnregisterContextualMenuAction(contextualMenuActionId);
		}
		_contextualMenuActionIds.Clear();

		for (uint32_t dropActionId : _dropActionIds)
		{
			AssetBrowserWindow::UnregisterDropAction(dropActionId);
		}
		_dropActionIds.Clear();

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
