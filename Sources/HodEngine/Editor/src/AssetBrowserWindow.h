#pragma once

#include <HodEngine/ImGui/src/Window.h>

namespace hod::editor
{
	/// @brief 
	class AssetBrowserWindow : public imgui::Window
	{
	public:

					AssetBrowserWindow() = default;
					~AssetBrowserWindow() override = default;

	public:

		void		Draw() override;

	private:

		void		DrawFolderTree();
		void		ResyncFolderTree();

		void		DrawFolderExplorer();
		void		ResyncFolderExplorer();

	private:


	};
}
