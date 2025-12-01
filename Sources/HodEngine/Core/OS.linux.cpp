#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <cassert>

namespace hod
{
	bool OS::OpenFileWithDefaultApp(const char* filePath)
	{
		return std::system(std::format("xdg-open {}", filePath).c_str()) == 0;
	}
}
