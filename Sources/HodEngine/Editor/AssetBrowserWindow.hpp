#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window/Window.hpp>

#include <vector>
#include <filesystem>

namespace hod::editor
{
	/// @brief 
	class HOD_API AssetBrowserWindow : public imgui::Window
	{
		META_TYPE(AssetBrowserWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					AssetBrowserWindow();
					~AssetBrowserWindow() override = default;

	public:

		void		DrawContent() override;

		void		PingAsset(Asset& asset);
		
	private:

		struct HOD_API FolderItem
		{
			std::filesystem::path		_path;
			bool						_expanded = false;

			std::vector<FolderItem*>	_children;
		};

		enum PingAnimState
		{
			In,
			Out,
		};

	private:

		void		DrawFolderTree();
		void		DrawFolderTreeNode(AssetDatabase::FileSystemMapping* node);
		void		ResyncFolderTree(FolderItem* folderItem);

		void		DrawFolderExplorer();
		bool		DrawExplorerItem(AssetDatabase::FileSystemMapping* item);
		void		ResyncFolderExplorer();

		void		EditNodeName(AssetDatabase::FileSystemMapping* node);


	private:

		FolderItem								_folderTree;
		AssetDatabase::FileSystemMapping*		_currentFolderTreeNode = nullptr;
		AssetDatabase::FileSystemMapping*		_treeNodeToEdit = nullptr;
		std::string								_renameBuffer;
		bool									_focus = false;

		AssetDatabase::FileSystemMapping* 		_nodeToDelete = nullptr;
		AssetDatabase::FileSystemMapping*		_currentExplorerNode = nullptr;

		AssetDatabase::FileSystemMapping* 		_itemToRename = nullptr;
		char									_itemRenameBuffer[256] = { '\0' };

		AssetDatabase::FileSystemMapping* 		_itemToPing = nullptr;
		PingAnimState							_pingAnimState;
		float									_pingAnimValue;
	};
}
