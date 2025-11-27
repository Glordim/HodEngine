#pragma once
#include "HodEngine/Core/Export.hpp"

namespace hod
{
	// Macro ensures the breakpoint triggers in caller code
	#if defined(_MSC_VER) || (defined(_WIN32) && defined(__clang__))
		#define Break() __debugbreak()
	#elif defined(__clang__)
		#define Break() __builtin_debugtrap()
	#else
		#if defined(__x86_64__)
			#define Break() __asm__("int3")
		#elif defined(__aarch64__)
			#define Break() __asm__("brk #0")
		#else
			#error
		#endif
	#endif

	HOD_CORE_API bool IsDebuggerAttached();
}
