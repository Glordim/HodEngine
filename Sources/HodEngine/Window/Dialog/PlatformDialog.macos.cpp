#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

namespace hod::window
{
	/// @brief 
	/// @return 
	std::filesystem::path GetSaveFileDialog(const std::string_view& typeName, const std::string_view& typeExtension, const std::filesystem::path& initialFolder)
	{
		char buffer[4096] = { '\0' };

		// TODO
		(void)typeName;
		(void)typeExtension;
		(void)initialFolder;

		return buffer;
	}

	/// @brief 
	/// @return 
	std::filesystem::path GetFolderDialog()
	{
		// TODO

		return std::filesystem::path();
	}

	/// @brief 
	/// @param path 
	void OpenExplorerAtPath(const std::filesystem::path& path)
	{
		// TODO
		(void)path;
	}
}
