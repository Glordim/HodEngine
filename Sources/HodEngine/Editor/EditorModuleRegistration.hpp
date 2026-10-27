#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <string_view>

namespace hod::inline editor
{
	/// @brief What an editor module adds to the editor (importers, cookers, resources, tabs, asset browser actions).
	/// Registered through it in HOD_STARTUP_MODULE, all of it is unregistered by UnregisterAll in HOD_SHUTDOWN_MODULE
	/// (or the destructor): the module code goes away on unload, "Hot Reload" included.
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
		template<typename _Importer_, typename... Args>
		void RegisterImporter(Args... extensions);

		template<typename _Cooker_>
		void RegisterCooker(std::string_view assetType);

		template<typename _Resource_>
		void RegisterResource(std::string_view assetType);

		template<typename _EditorTabType_>
		void RegisterEditorTab(std::string_view assetType);

		void RegisterContextualMenuAction(AssetBrowserWindow::ContextualMenuAction contextualMenuAction);

		/// @brief In the reverse order of the registrations
		void UnregisterAll();

	private:
		Vector<void (*)()> _importerUnregisters; // One per importer type, the importers being identified by their type
		Vector<String>     _cookerAssetTypes;
		Vector<String>     _resourceAssetTypes;
		Vector<String>     _editorTabAssetTypes;
		Vector<uint32_t>   _contextualMenuActionIds;
	};

	template<typename _Importer_, typename... Args>
	void EditorModuleRegistration::RegisterImporter(Args... extensions)
	{
		AssetDatabase::GetInstance()->RegisterImporter<_Importer_>(extensions...);
		_importerUnregisters.PushBack(+[]() { AssetDatabase::GetInstance()->UnregisterImporter<_Importer_>(); });
	}

	template<typename _Cooker_>
	void EditorModuleRegistration::RegisterCooker(std::string_view assetType)
	{
		AssetDatabase::GetInstance()->RegisterCooker<_Cooker_>(assetType);
		_cookerAssetTypes.PushBack(String(assetType));
	}

	template<typename _Resource_>
	void EditorModuleRegistration::RegisterResource(std::string_view assetType)
	{
		AssetDatabase::GetInstance()->RegisterResource<_Resource_>(assetType);
		_resourceAssetTypes.PushBack(String(assetType));
	}

	template<typename _EditorTabType_>
	void EditorModuleRegistration::RegisterEditorTab(std::string_view assetType)
	{
		Editor::GetInstance()->RegisterEditorTab<_EditorTabType_>(assetType);
		_editorTabAssetTypes.PushBack(String(assetType));
	}
}
