#pragma once

#include <HodEngine/Core/Singleton.h>

#include <filesystem>

namespace hod::application::dialog
{
	std::filesystem::path	GetOpenFileDialog();
	std::filesystem::path	GetSaveFileDialog();

	std::filesystem::path	GetFolderDialog();

	void					OpenExplorerAtPath(const std::filesystem::path& path);
}
