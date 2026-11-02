#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/Editor/AssetType.hpp"
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
	/// @param assetType
	/// @return
	bool EditorModuleRegistration::RegisterAssetType(AssetType* assetType)
	{
		if (AssetDatabase::GetInstance()->RegisterAssetType(*assetType) == false)
		{
			DefaultAllocator::GetInstance().Delete(assetType);
			return false;
		}
		_assetTypes.PushBack(assetType);
		assetType->OnRegistered(*this);
		return true;
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
	/// @param enumDescriptor
	/// @param preview
	/// @return
	bool EditorModuleRegistration::RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, PropertyDrawer::EnumValuePreview preview)
	{
		if (PropertyDrawer::RegisterEnumValuePreview(enumDescriptor, preview) == false)
		{
			return false;
		}
		_enumValuePreviewDescriptors.PushBack(&enumDescriptor);
		return true;
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

		for (const EnumDescriptor* enumDescriptor : _enumValuePreviewDescriptors)
		{
			PropertyDrawer::UnregisterEnumValuePreview(*enumDescriptor);
		}
		_enumValuePreviewDescriptors.Clear();

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

		// The importers before the asset types they import into
		for (void (*importerUnregister)() : _importerUnregisters)
		{
			importerUnregister();
		}
		_importerUnregisters.Clear();

		// Their tabs first: closing them can still use the asset types
		for (AssetType* assetType : _assetTypes)
		{
			Editor::GetInstance()->CloseEditorTabsOf(assetType->GetHashV());
		}
		for (AssetType* assetType : _assetTypes)
		{
			assetType->OnUnregistered();
			AssetDatabase::GetInstance()->UnregisterAssetType(*assetType);
			DefaultAllocator::GetInstance().Delete(assetType);
		}
		_assetTypes.Clear();
	}
}
