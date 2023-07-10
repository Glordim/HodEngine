#pragma once
#include "HodEngine/Editor/AssetDatabase.h"
#include <HodEngine/ImGui/Window.h>

#include <vector>
#include <filesystem>

namespace hod::editor
{
	/// @brief 
	class AssetBrowserWindow : public imgui::Window
	{
		META_TYPE(AssetBrowserWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					AssetBrowserWindow();
					~AssetBrowserWindow() override = default;

	public:

		void		Draw() override;

	private:

		struct FolderItem
		{
			std::filesystem::path		_path;
			bool						_expanded = false;

			std::vector<FolderItem*>	_children;
		};

	private:

		void		DrawFolderTree();
		void		DrawFolderTreeNode(const AssetDatabase::FileSystemMapping* node);
		void		ResyncFolderTree(FolderItem* folderItem);

		void		DrawFolderExplorer();
		bool		DrawExplorerItem(const AssetDatabase::FileSystemMapping* item);
		void		ResyncFolderExplorer();

		void		EditNodeName(const AssetDatabase::FileSystemMapping* node);

	private:

		FolderItem								_folderTree;
		const AssetDatabase::FileSystemMapping*	_currentFolderTreeNode = nullptr;
		const AssetDatabase::FileSystemMapping*	_treeNodeToEdit = nullptr;
		std::string								_renameBuffer;
		bool									_focus = false;

		const AssetDatabase::FileSystemMapping* _nodeToDelete = nullptr;
		const AssetDatabase::FileSystemMapping*	_currentExplorerNode = nullptr;
	};
}
