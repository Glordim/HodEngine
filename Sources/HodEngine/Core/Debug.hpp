#pragma once
#include "HodEngine/Core/Export.hpp"

namespace hod
{
	// Macro ensures the breakpoint triggers in caller code
#if defined(PLATFORM_WINDOWS)
	#define Break() __debugbreak();
#else
	#define Break() __builtin_debugtrap();
#endif

	HOD_CORE_API bool IsDebuggerAttached();
}
