#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod::inline core
{
	class HOD_CORE_API OS
	{
	public:
		static bool ReadClipboard(String& value);
		static bool WriteClipboard(const std::string_view& value);

		static bool OpenFileWithDefaultApp(const char* filePath);
		static bool SetEnv(const char* name, const char* value);

#if defined(PLATFORM_WINDOWS)
		static String GetLastWin32ErrorMessage();
#endif
		static const char* GetErrnoMessage();
	};
}
