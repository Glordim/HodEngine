#pragma once

#include <HodEngine/Core/Singleton.h>

#include <filesystem>

namespace hod::application::dialog
{
	std::filesystem::path	GetOpenFileDialog();
	std::filesystem::path	GetSaveFileDialog();

	void					OpenExplorerAtPath(const std::filesystem::path& path);
}