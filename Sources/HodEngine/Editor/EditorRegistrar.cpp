#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorRegistrar.hpp"
#include "HodEngine/Editor/AssetType.hpp"
#include "HodEngine/Editor/PropertyDrawer/PropertyDrawerRegistry.hpp"
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
	bool EditorRegistrar::RegisterPropertyDrawer(const ReflectionDescriptor& descriptor, PropertyDrawerBase* drawer)
	{
		if (PropertyDrawerRegistry::Register(descriptor, drawer) == false)
		{
			return false;
		}
		_registration.AddUnregister([&descriptor]() { PropertyDrawerRegistry::Unregister(descriptor); });
		return true;
	}

	/// @brief
	/// @param enumDescriptor
	/// @param preview
	/// @return
	bool EditorRegistrar::RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, ReflectionDrawer::EnumValuePreview preview)
	{
		if (ReflectionDrawer::RegisterEnumValuePreview(enumDescriptor, preview) == false)
		{
			return false;
		}
		_registration.AddUnregister([&enumDescriptor]() { ReflectionDrawer::UnregisterEnumValuePreview(enumDescriptor); });
		return true;
	}

	/// @brief
	/// @param route
	/// @param descriptor
	bool EditorRegistrar::RegisterWindow(const String& route, ReflectionDescriptor& descriptor)
	{
		if (WindowFactory::GetInstance()->Register(route, descriptor) == false)
		{
			return false;
		}
		_registration.AddUnregister([&descriptor]() { WindowFactory::GetInstance()->Unregister(descriptor); });
		return true;
	}
}
