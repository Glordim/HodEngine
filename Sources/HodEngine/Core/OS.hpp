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

#if defined(PLATFORM_WINDOWS)
		static std::string GetLastWin32ErrorMessage();
#endif
	};
}
