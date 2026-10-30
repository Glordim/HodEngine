#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <cstdint>
#include <functional>

namespace hod::inline editor
{
	/// @brief
	class HOD_EDITOR_API AssetBrowserWindow : public imgui::Window
	{
		REFLECTED_CLASS(AssetBrowserWindow, imgui::Window)

	public:
		AssetBrowserWindow();
		~AssetBrowserWindow() override = default;

	public:
		bool Draw() override;
		void DrawContent() override;

		void PingAsset(Asset& asset);

		void Rename(const Path& path);

	public:
		struct Context
		{
			AssetBrowserWindow& assetBrowserWindow;
			Path currentDirectory;
			Vector<Path> selectedItems;
		};

		struct ContextualMenuAction
		{
			String path;
			String group;
			std::function<bool(const Context&)> available;
			std::function<void(const Context&)> execute;
			uint32_t id = 0; // Given by RegisterContextualMenuAction, 0 for the ones of the editor itself (PopulateContextualMenuActions)
		};

		/// @brief By the editor modules (registered again when they reload): kept when the actions of the editor are populated again
		static uint32_t RegisterContextualMenuAction(ContextualMenuAction contextualMenuAction);
		static void UnregisterContextualMenuAction(uint32_t contextualMenuActionId);

		static void PopulateContextualMenuActions();

		/// @brief What a drag and drop payload of another window (e.g. a node of a hierarchy) does dropped on a folder of the
		/// asset browser (the explorer's background being the current folder), typically making an asset of it
		struct DropAction
		{
			String                                                                         payloadType; // ImGui payload type
			std::function<bool(const void* payloadData)>                                   available;   // Optional: highlighted and accepted only then
			std::function<void(const void* payloadData, const Path& folder, AssetBrowserWindow&)> execute;     // Out of the browser's drawing
			uint32_t                                                                       id = 0;      // Given by RegisterDropAction
		};

		/// @brief By the editor modules (through EditorModuleRegistration)
		static uint32_t RegisterDropAction(DropAction dropAction);
		static void     UnregisterDropAction(uint32_t dropActionId);

	private:
		struct HOD_EDITOR_API FolderItem
		{
			Path _path;
			bool _expanded = false;

			Vector<FolderItem*> _children;
		};

		enum PingAnimState
		{
			In,
			Out,
		};

		enum class Panel
		{
			FolderTree,
			FolderExplorer,
		};

	private:
		String GetPanelIdentifier(Panel panel) const;
		void   CreateDefaultLayout();
		bool   RedockFloatingPanels();
		void   DrawPanel(Panel panel, void (AssetBrowserWindow::*drawContent)());

		void DrawToolbar();
		void DrawPath();
		void DrawCreateMenu();

		void DrawFolderTree();
		void DrawFolderTreeNode(AssetDatabase::FileSystemMapping* node);
		void ResyncFolderTree(FolderItem* folderItem);

		void DrawFolderExplorer();
		bool DrawExplorerItem(AssetDatabase::FileSystemMapping* item);
		void ResyncFolderExplorer();

		void EditNodeName(AssetDatabase::FileSystemMapping* node);
		void StartItemRename(AssetDatabase::FileSystemMapping* item);
		void ApplyRename(AssetDatabase::FileSystemMapping& item, const char* newName);

		bool         IsSelected(const AssetDatabase::FileSystemMapping* item) const;
		void         SetSelected(AssetDatabase::FileSystemMapping* item, bool selected);
		void         PruneSelection(const Vector<AssetDatabase::FileSystemMapping*>& explorerItems);
		void         ApplySelectionRequests(ImGuiMultiSelectIO* multiSelectIO, const Vector<AssetDatabase::FileSystemMapping*>& explorerItems);
		Vector<Path> GetSelectedPaths() const;

		void DrawFolderDropTarget(AssetDatabase::FileSystemMapping* folder);
		void AcceptFolderDropPayloads(AssetDatabase::FileSystemMapping* folder);
		void ApplyPendingDrop();

	private:
		ImGuiID                           _dockSpaceId = 0;
		ImGuiID                           _panelDockIds[2] = {0, 0}; // Last dock node of each Panel, to put it back when dropped outside
		FolderItem                        _folderTree;
		AssetDatabase::FileSystemMapping* _currentFolderTreeNode = nullptr;
		AssetDatabase::FileSystemMapping* _revealedFolderTreeNode = nullptr; // Last selection expanded up to in the tree
		bool                              _revealCurrentFolderTreeNode = false;
		AssetDatabase::FileSystemMapping* _treeNodeToEdit = nullptr;
		String                            _renameBuffer;
		bool                              _focus = false;

		AssetDatabase::FileSystemMapping* _nodeToDelete = nullptr;

		Vector<AssetDatabase::FileSystemMapping*> _selectedItems;             // Explorer multi-selection, items of _selectionFolder only
		AssetDatabase::FileSystemMapping*         _selectionFolder = nullptr;
		Vector<Path>                              _contextualMenuSelection;   // Selection the explorer contextual menu was opened on

		AssetDatabase::FileSystemMapping* _explorerDraggedItem = nullptr; // Drag source of the explorer, its drop moves the whole selection
		AssetDatabase::FileSystemMapping* _pendingDropItem = nullptr;
		AssetDatabase::FileSystemMapping* _pendingDropFolder = nullptr;
		uint32_t                          _pendingDropActionId = 0; // A DropAction's drop, its payload copied (ImGui frees it)
		Vector<uint8_t>                   _pendingDropActionData;

		AssetDatabase::FileSystemMapping* _itemToRename = nullptr;
		char                              _itemRenameBuffer[256] = {'\0'};
		bool                              _itemRenameFocus = false;
		bool                              _itemRenameSubmitted = false; // Its field was drawn this frame
		bool                              _treeRenameSubmitted = false; // Same for _treeNodeToEdit

		AssetDatabase::FileSystemMapping* _itemToPing = nullptr;
		PingAnimState                     _pingAnimState;
		float                             _pingAnimValue;

		static Vector<ContextualMenuAction> _contextualMenuActions;
		static uint32_t                     _nextContextualMenuActionId;
		static bool                         _populatingContextualMenuActions;
		static uint32_t                     _contextualMenuActionsVersion; // Changed with _contextualMenuActions: the items pointing into it are stale

		static Vector<DropAction> _dropActions;
		static uint32_t           _nextDropActionId;

		struct ContextualMenuItem
		{
			String label;
			bool isSeparator;
			// Not a copy: the lambdas of an editor module must not outlive its unload (hot reload, editor shutdown)
			const std::function<void(const Context&)>* callback = nullptr;
		};
		Vector<ContextualMenuItem> _contextualMenuData;
		uint32_t                   _contextualMenuDataVersion = 0;

		static const ContextualMenuItem* DrawContextualMenuItems(const Vector<ContextualMenuItem>& menuItems, std::string_view prefix);
	};
}
