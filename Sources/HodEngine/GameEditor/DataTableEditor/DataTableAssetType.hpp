#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"
#include "HodEngine/Editor/EditorModuleRegistration.hpp"

#include <HodEngine/Core/Event.hpp>

namespace hod::inline editor
{
	/// @brief Rows of a data struct of the project modules (see DataStructFactory)
	class HOD_GAME_EDITOR_API DataTableAssetType : public AssetType
	{
		HOD_ASSET_TYPE("DataTable", "datatable")

	public:
		DataTableAssetType();

		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		Texture*                    GetIcon() const override;
		void                        OnRegistered(EditorModuleRegistration& registration) override;
		void                        OnUnregistered() override;

	private:
		void RegisterCreateActions();

	private:
		// "Create/DataTable/<data struct>", one per data struct of the project modules: registered again when they reload
		EditorModuleRegistration _createActionsRegistration;
		Event<>::Slot            _onProjectModulesReloadedSlot; // After _createActionsRegistration: disconnected first
	};
}
