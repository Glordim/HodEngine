#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorRegistrar.hpp"
#include "HodEngine/Editor/AssetType.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"
#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/VersionControl/VersionControl.hpp"
#include "HodEngine/Editor/WindowFactory.hpp"

namespace hod::inline editor
{
	/// @brief
	/// @param registration
	EditorRegistrar::EditorRegistrar(ModuleRegistration& registration)
	: _registration(registration)
	{
	}

	/// @brief
	/// @param assetType
	/// @return
	bool EditorRegistrar::RegisterAssetType(AssetType* assetType)
	{
		if (AssetDatabase::GetInstance()->RegisterAssetType(*assetType) == false)
		{
			DefaultAllocator::GetInstance().Delete(assetType);
			return false;
		}
		_registration.AddUnregister(
			[assetType]()
			{
				// Its tabs first: closing them can still use the asset type
				Editor::GetInstance()->CloseEditorTabsOf(assetType->GetHashV());
				assetType->OnUnregistered();
				AssetDatabase::GetInstance()->UnregisterAssetType(*assetType);
				DefaultAllocator::GetInstance().Delete(assetType);
			});
		assetType->OnRegistered(*this); // What it adds goes before it (registered after it)
		return true;
	}

	/// @brief
	/// @param provider
	/// @return
	bool EditorRegistrar::RegisterVersionControlProvider(VersionControlProvider* provider)
	{
		if (VersionControl::GetInstance()->RegisterProvider(*provider) == false)
		{
			OUTPUT_ERROR("EditorRegistrar::RegisterVersionControlProvider: a version control provider named {} is registered already", provider->GetName());
			DefaultAllocator::GetInstance().Delete(provider);
			return false;
		}
		_registration.AddUnregister(
			[provider]()
			{
				VersionControl::GetInstance()->UnregisterProvider(*provider); // Disconnected if it is the connected one, its worker stopped
				DefaultAllocator::GetInstance().Delete(provider);
			});
		return true;
	}

	/// @brief
	/// @param contextualMenuAction
	void EditorRegistrar::RegisterContextualMenuAction(AssetBrowserWindow::ContextualMenuAction contextualMenuAction)
	{
		uint32_t id = AssetBrowserWindow::RegisterContextualMenuAction(std::move(contextualMenuAction));
		_registration.AddUnregister([id]() { AssetBrowserWindow::UnregisterContextualMenuAction(id); });
	}

	/// @brief
	/// @param dropAction
	void EditorRegistrar::RegisterDropAction(AssetBrowserWindow::DropAction dropAction)
	{
		uint32_t id = AssetBrowserWindow::RegisterDropAction(std::move(dropAction));
		_registration.AddUnregister([id]() { AssetBrowserWindow::UnregisterDropAction(id); });
	}

	/// @brief
	/// @param descriptor
	/// @param drawer
	void EditorRegistrar::RegisterCustomPropertyDrawer(const ReflectionDescriptor& descriptor, CustomPropertyDrawer* drawer)
	{
		CustomPropertyDrawerRegistry::Register(descriptor, drawer);
		_registration.AddUnregister([&descriptor]() { CustomPropertyDrawerRegistry::Unregister(descriptor); });
	}

	/// @brief
	/// @param descriptor
	/// @param drawer
	void EditorRegistrar::RegisterCustomComponentDrawer(const ReflectionDescriptor& descriptor, CustomComponentDrawer* drawer)
	{
		CustomComponentDrawerRegistry::Register(descriptor, drawer);
		_registration.AddUnregister([&descriptor]() { CustomComponentDrawerRegistry::Unregister(descriptor); });
	}

	/// @brief
	/// @param enumDescriptor
	/// @param preview
	/// @return
	bool EditorRegistrar::RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, PropertyDrawer::EnumValuePreview preview)
	{
		if (PropertyDrawer::RegisterEnumValuePreview(enumDescriptor, preview) == false)
		{
			return false;
		}
		_registration.AddUnregister([&enumDescriptor]() { PropertyDrawer::UnregisterEnumValuePreview(enumDescriptor); });
		return true;
	}

	/// @brief
	/// @param route
	/// @param descriptor
	void EditorRegistrar::RegisterWindow(const String& route, ReflectionDescriptor& descriptor)
	{
		WindowFactory::GetInstance()->Register(route, descriptor);
		_registration.AddUnregister([&descriptor]() { WindowFactory::GetInstance()->Unregister(descriptor); });
	}
}
