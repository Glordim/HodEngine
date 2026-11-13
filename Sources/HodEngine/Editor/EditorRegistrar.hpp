#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/ModuleRegistration.hpp>
#include <HodEngine/Core/String.hpp>

namespace hod::inline core
{
	class EnumDescriptor;
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class AssetType;
	class CustomPropertyDrawer;
	class CustomComponentDrawer;
	class VersionControlProvider;

	/// @brief The registrar of the editor (see ModuleRegistration): importers, asset types, asset browser actions, drawers, windows, version
	/// control providers, unregistered with the rest of what the module registered, before it unloads ("Hot Reload" included). In an
	/// editor module's startup (see HOD_DECLARE_EDITOR_MODULE). Only what was actually registered is unregistered: what another module has
	/// registered already is refused, not taken over.
	class HOD_EDITOR_API EditorRegistrar
	{
	public:
		explicit EditorRegistrar(ModuleRegistration& registration);

	public:
		/// @brief An instance of _AssetType_ (see AssetType), owned by the registration, then what it adds (AssetType::OnRegistered)
		/// @return false (with an error) if the type is registered already (by another module): nothing registered then
		template<typename _AssetType_>
		bool RegisterAssetType();

		/// @brief After the asset type it imports into (see Importer::SetAssetType)
		/// @return false (with an error) if one of the extensions has an importer already
		template<typename _Importer_, typename... Args>
		bool RegisterImporter(Args... extensions);

		void RegisterContextualMenuAction(AssetBrowserWindow::ContextualMenuAction contextualMenuAction);
		void RegisterDropAction(AssetBrowserWindow::DropAction dropAction);

		template<typename _Type_, typename _Drawer_>
		void RegisterCustomPropertyDrawer();
		/// @param drawer Owned by the registry
		void RegisterCustomPropertyDrawer(const ReflectionDescriptor& descriptor, CustomPropertyDrawer* drawer);

		template<typename _Type_, typename _Drawer_>
		void RegisterCustomComponentDrawer();
		/// @param drawer Owned by the registry
		void RegisterCustomComponentDrawer(const ReflectionDescriptor& descriptor, CustomComponentDrawer* drawer);

		/// @brief See PropertyDrawer::EnumValuePreview
		/// @return false (with an error) if the enum has a preview already (from another module)
		bool RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, PropertyDrawer::EnumValuePreview preview);

		/// @brief In the "Window" menu, under route
		template<typename _Window_>
		void RegisterWindow(const String& route);
		void RegisterWindow(const String& route, ReflectionDescriptor& descriptor);

		/// @brief An instance of _Provider_ (see VersionControlProvider), owned by the registration, offered to the user's choice
		/// (see VersionControl::SetProviderChoice). Disconnected before its module unloads if it was the connected one
		/// @return false (with an error) if a provider of the same name is registered already
		template<typename _Provider_>
		bool RegisterVersionControlProvider();

	private:
		/// @param assetType Owned by the registration, deleted right away if refused
		bool RegisterAssetType(AssetType* assetType);
		/// @param provider Owned by the registration, deleted right away if refused
		bool RegisterVersionControlProvider(VersionControlProvider* provider);

	private:
		ModuleRegistration& _registration;
	};

	template<typename _AssetType_>
	bool EditorRegistrar::RegisterAssetType()
	{
		return RegisterAssetType(DefaultAllocator::GetInstance().New<_AssetType_>());
	}

	template<typename _Provider_>
	bool EditorRegistrar::RegisterVersionControlProvider()
	{
		return RegisterVersionControlProvider(DefaultAllocator::GetInstance().New<_Provider_>());
	}

	template<typename _Importer_, typename... Args>
	bool EditorRegistrar::RegisterImporter(Args... extensions)
	{
		if (AssetDatabase::GetInstance()->RegisterImporter<_Importer_>(extensions...) == false)
		{
			return false;
		}
		_registration.AddUnregister([]() { AssetDatabase::GetInstance()->UnregisterImporter<_Importer_>(); });
		return true;
	}

	template<typename _Type_, typename _Drawer_>
	void EditorRegistrar::RegisterCustomPropertyDrawer()
	{
		RegisterCustomPropertyDrawer(_Type_::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Drawer_>());
	}

	template<typename _Type_, typename _Drawer_>
	void EditorRegistrar::RegisterCustomComponentDrawer()
	{
		RegisterCustomComponentDrawer(_Type_::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Drawer_>());
	}

	template<typename _Window_>
	void EditorRegistrar::RegisterWindow(const String& route)
	{
		RegisterWindow(route, _Window_::GetReflectionDescriptor());
	}
}
