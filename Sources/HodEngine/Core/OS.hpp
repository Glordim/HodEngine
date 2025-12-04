#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod
{
	class HOD_CORE_API OS
	{
	public:
		static bool OpenFileWithDefaultApp(const char* filePath);

#if defined(PLATFORM_WINDOWS)
		static String GetLastWin32ErrorMessage();
#endif
	};
}
