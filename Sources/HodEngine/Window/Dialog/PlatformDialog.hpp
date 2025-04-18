#pragma once
#include "HodEngine/Window/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include <filesystem>

namespace hod::window
{
	HOD_WINDOW_API std::filesystem::path	GetOpenFileDialog();
	HOD_WINDOW_API std::filesystem::path	GetSaveFileDialog(const std::string_view& typeName, const std::string_view& typeExtension, const std::filesystem::path& initialFolder = "");

	HOD_WINDOW_API std::filesystem::path	GetFolderDialog();

	HOD_WINDOW_API void					OpenExplorerAtPath(const std::filesystem::path& path);

	HOD_WINDOW_API bool					ShowYesNoDialog(const std::string_view& title, const std::string_view& description, const std::string_view& yesChoice = "Yes", const std::string_view& noChoice = "No");
}
