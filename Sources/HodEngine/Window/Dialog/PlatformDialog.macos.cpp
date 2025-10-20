#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

namespace hod::window
{
	/// @brief
	/// @return
	Path GetSaveFileDialog(const std::string_view& typeName, const std::string_view& typeExtension, const Path& initialFolder)
	{
		char buffer[4096] = {'\0'};

		// TODO
		(void)typeName;
		(void)typeExtension;
		(void)initialFolder;

		return buffer;
	}

	/// @brief
	/// @return
	Path GetFolderDialog()
	{
		// TODO

		return Path();
	}

	/// @brief
	/// @param path
	void OpenExplorerAtPath(const Path& path)
	{
		// TODO
		(void)path;
	}
}
