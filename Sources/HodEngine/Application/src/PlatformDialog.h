#pragma once

#include <HodEngine/Core/Src/Singleton.h>

#include <filesystem>

namespace hod::application::dialog
{
	std::filesystem::path GetOpenFileDialog();
	std::filesystem::path GetSaveFileDialog();
}
