#pragma once

#include <HodEngine/ImGui/src/Window.h>

#include <vector>
#include <filesystem>

namespace hod::editor
{
	/// @brief 
	class AssetBrowserWindow : public imgui::Window
	{
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
		void		ResyncFolderTree(FolderItem* folderItem);

		void		DrawFolderExplorer();
		void		ResyncFolderExplorer();

	private:

		FolderItem	_folderTree;
	};
}
