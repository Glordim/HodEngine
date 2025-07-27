#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Core/Vector.hpp"
#include <filesystem>

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API AssetBrowserWindow : public imgui::Window
	{
		REFLECTED_CLASS(AssetBrowserWindow, imgui::Window)

	public:

					AssetBrowserWindow();
					~AssetBrowserWindow() override = default;

	public:

		bool		Draw() override;
		void		DrawContent() override;

		void		PingAsset(Asset& asset);
		
	private:

		struct HOD_EDITOR_API FolderItem
		{
			std::filesystem::path		_path;
			bool						_expanded = false;

			Vector<FolderItem*>	_children;
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
