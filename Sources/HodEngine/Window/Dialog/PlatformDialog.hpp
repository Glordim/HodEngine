#pragma once
#include "HodEngine/Window/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include <HodEngine/Core/FileSystem/Path.hpp>

namespace hod::window
{
	HOD_WINDOW_API Path GetOpenFileDialog();
	HOD_WINDOW_API Path GetSaveFileDialog(const std::string_view& typeName, const std::string_view& typeExtension, const Path& initialFolder = "");

	HOD_WINDOW_API Path GetFolderDialog();

	HOD_WINDOW_API void OpenExplorerAtPath(const Path& path);

	HOD_WINDOW_API bool ShowYesNoDialog(const std::string_view& title, const std::string_view& description, const std::string_view& yesChoice = "Yes",
	                                    const std::string_view& noChoice = "No");
}
