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

	class HOD_CORE_API Debug
	{
	public:
		struct SymbolInfo
		{
			String   _module;
			String   _function;
			void*    _address;
			uint32_t _line;
		};

	public:
		static bool IsDebuggerAttached();

		static uint32_t GetCallstack(void** callstack, uint32_t maxSize); // todo span ?
		static String   GetSymbol(void* addr);
		static bool     GetSymbolInfo(void* addr, SymbolInfo& symbolInfo, bool demangle);
	};
}
