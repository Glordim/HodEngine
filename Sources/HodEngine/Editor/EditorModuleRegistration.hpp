#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

#include <cstdint>
#include <string_view>

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

	/// @brief What an editor module adds to the editor (importers, asset types, asset browser actions, drawers, windows). Given
	/// to the module's startup by the editor (see HOD_DECLARE_EDITOR_MODULE), which unregisters all of it (UnregisterAll, or the
	/// destructor) before unloading the module: its code goes away, "Hot Reload" included. Only what was actually registered is
	/// unregistered: what another module has registered already is refused, not taken over.
	class HOD_EDITOR_API EditorModuleRegistration
	{
	public:
		EditorModuleRegistration() = default;
		EditorModuleRegistration(const EditorModuleRegistration&) = delete;
		EditorModuleRegistration(EditorModuleRegistration&&) = delete;
		~EditorModuleRegistration();

		EditorModuleRegistration& operator=(const EditorModuleRegistration&) = delete;
		EditorModuleRegistration& operator=(EditorModuleRegistration&&) = delete;

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
		void RegisterCustomPropertyDrawer(const ReflectionDescriptor& descriptor, CustomPropertyDrawer* drawer);

		template<typename _Type_, typename _Drawer_>
		void RegisterCustomComponentDrawer();
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

		/// @brief In the reverse order of the registrations
		void UnregisterAll();

	private:
		/// @param assetType Owned by the registration, deleted right away if refused
		bool RegisterAssetType(AssetType* assetType);
		/// @param provider Owned by the registration, deleted right away if refused
		bool RegisterVersionControlProvider(VersionControlProvider* provider);

	private:
		Vector<VersionControlProvider*> _versionControlProviders;
		Vector<void (*)()> _importerUnregisters; // One per importer type, the importers being identified by their type
		Vector<AssetType*> _assetTypes;
		Vector<uint32_t>   _contextualMenuActionIds;
		Vector<uint32_t>   _dropActionIds;

		Vector<const ReflectionDescriptor*> _customPropertyDrawerDescriptors;
		Vector<const ReflectionDescriptor*> _customComponentDrawerDescriptors;
		Vector<const EnumDescriptor*>       _enumValuePreviewDescriptors;
		Vector<ReflectionDescriptor*>       _windowDescriptors;
	};

	template<typename _AssetType_>
	bool EditorModuleRegistration::RegisterAssetType()
	{
		return RegisterAssetType(DefaultAllocator::GetInstance().New<_AssetType_>());
	}

	template<typename _Provider_>
	bool EditorModuleRegistration::RegisterVersionControlProvider()
	{
		return RegisterVersionControlProvider(DefaultAllocator::GetInstance().New<_Provider_>());
	}

	template<typename _Importer_, typename... Args>
	bool EditorModuleRegistration::RegisterImporter(Args... extensions)
	{
		if (AssetDatabase::GetInstance()->RegisterImporter<_Importer_>(extensions...) == false)
		{
			return false;
		}
		_importerUnregisters.PushBack(+[]() { AssetDatabase::GetInstance()->UnregisterImporter<_Importer_>(); });
		return true;
	}

	template<typename _Type_, typename _Drawer_>
	void EditorModuleRegistration::RegisterCustomPropertyDrawer()
	{
		RegisterCustomPropertyDrawer(_Type_::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Drawer_>());
	}

	template<typename _Type_, typename _Drawer_>
	void EditorModuleRegistration::RegisterCustomComponentDrawer()
	{
		RegisterCustomComponentDrawer(_Type_::GetReflectionDescriptor(), DefaultAllocator::GetInstance().New<_Drawer_>());
	}

	template<typename _Window_>
	void EditorModuleRegistration::RegisterWindow(const String& route)
	{
		RegisterWindow(route, _Window_::GetReflectionDescriptor());
	}
}
