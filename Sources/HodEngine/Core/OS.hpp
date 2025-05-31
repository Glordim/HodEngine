#pragma once
#include "HodEngine/Core/Export.hpp"

#include <string>

namespace hod
{
	class HOD_CORE_API OS
	{
	public:

		static uint32_t		GetCallstack(void** callstack, uint32_t maxSize); // todo span ?
		static std::string	GetSymbol(void* addr);
		static bool			GetSymbol(void* addr, char* symbol, uint32_t size);

		static void			OpenFileWithDefaultApp(const char* filePath);

#if defined(PLATFORM_WINDOWS)
		static std::string GetLastWin32ErrorMessage();
#endif
	};
}
