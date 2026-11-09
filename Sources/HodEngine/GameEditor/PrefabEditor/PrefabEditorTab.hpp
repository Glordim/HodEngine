#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include <string_view>

namespace hod::inline core
{
	class Document;
	class DocumentNode;
	class Path;
	class UID;
}

namespace hod::inline editor
{
	/// @brief Editor tab of a prefab asset. A prefab has one root entity, the one its instances are: every other entity is under it.
	/// The root is named after the asset (renaming the asset renames it, see PrefabAssetType::OnRenamed), it can't be renamed, moved
	/// nor deleted. Created with it (see CreateEmptyPrefab).
	class HOD_GAME_EDITOR_API PrefabEditorTab : public EntityEditorTab
	{
	public:
		PrefabEditorTab(std::shared_ptr<Asset> asset);
		~PrefabEditorTab() override;

		Entity* GetRootEntity() const override;
		bool    CanInstantiatePrefab(const UID& prefab) const override;

		/// @brief Writes a new prefab asset at assetPath, with its root (named after the file) as its tab would create it
		static bool CreateEmptyPrefab(const Path& assetPath);

		/// @brief Names the root entity of the prefab document (the "Entities" data block of a prefab asset) after the asset
		/// @return true if the document changed, false if already named so or if it has no single root
		static bool NameRootEntityAfterAsset(Document& prefabDocument, std::string_view assetName);

		/// @brief Whether the prefab prefab instantiates the prefab other, itself or through the instances it holds (as last cooked)
		static bool ContainsPrefab(const UID& prefab, const UID& other);

	protected:
		void CreateDefaultLayout() override;
		bool DrawContent() override;
		bool OnSave() override;

	private:
		/// @brief The root follows the asset's name (renamed since the last frame), not an edit: the asset's file was renamed already
		void SyncRootEntityName();

		static Entity* CreateRootEntity(std::string_view name);
		static const DocumentNode* FindRootEntityNode(const Document& prefabDocument);
	};
}
