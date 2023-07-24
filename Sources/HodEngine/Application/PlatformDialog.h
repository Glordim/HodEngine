#pragma once

#include <HodEngine/Core/Singleton.h>

#include <filesystem>

namespace hod::application::dialog
{
	std::filesystem::path	GetOpenFileDialog();
	std::filesystem::path	GetSaveFileDialog(const std::string_view& typeName, const std::string_view& typeExtension, const std::filesystem::path& initialFolder = "");

	std::filesystem::path	GetFolderDialog();

	void					OpenExplorerAtPath(const std::filesystem::path& path);
}
