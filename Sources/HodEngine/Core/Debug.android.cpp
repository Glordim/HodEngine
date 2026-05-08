#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Debug.hpp"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unwind.h>

extern "C"
{
	char* __cxa_demangle(const char*, char*, size_t*, int*);
}

namespace hod::inline core
{
	bool Debug::IsDebuggerAttached()
	{
		FILE* file = fopen("/proc/self/status", "r");
		if (!file)
		{
			return false;
		}

		char line[256];
		while (fgets(line, sizeof(line), file))
		{
			if (strncmp(line, "TracerPid:", 10) == 0)
			{
				int tracerPid = atoi(line + 10);
				fclose(file);
				return tracerPid != 0;
			}
		}

		fclose(file);
		return false;
	}

	/// @brief
	/// @param callstack
	/// @param maxSize
	/// @return
	uint32_t Debug::GetCallstack(void** callstack, uint32_t maxSize)
	{
		struct BacktraceState
		{
			uint32_t _currentIndex = 0;
			void**   _callstack = nullptr;
			uint32_t _maxSize = 0;
		};

		BacktraceState state;
		state._currentIndex = 0;
		state._callstack = callstack;
		state._maxSize = maxSize;
		_Unwind_Backtrace(
			[](struct _Unwind_Context* context, void* arg)
			{
				BacktraceState* state = static_cast<BacktraceState*>(arg);
				uintptr_t       pc = _Unwind_GetIP(context);
				if (pc)
				{
					if (state->_currentIndex >= state->_maxSize)
					{
						return _URC_END_OF_STACK;
					}
					else
					{
						state->_callstack[state->_currentIndex] = reinterpret_cast<void*>(pc);
						state->_currentIndex += 1;
					}
				}
				return _URC_NO_REASON;
			},
			&state);
		return state._currentIndex;
	}

	/// @brief
	/// @param addr
	/// @return
	String Debug::GetSymbol(void* addr)
	{
		String symbol = "NOT_FOUND";

		Dl_info info;
		if (dladdr(addr, &info) && info.dli_sname)
		{
			symbol = info.dli_sname;
		}
		return symbol;
	}

	bool Debug::GetSymbolInfo(void* addr, SymbolInfo& symbolInfo, bool demangle)
	{
		Dl_info info;
		if (dladdr(addr, &info) == 0)
		{
			return false;
		}

		symbolInfo._address = info.dli_saddr;
		symbolInfo._module = info.dli_fname;
		symbolInfo._function = info.dli_sname;
		symbolInfo._line = 0;

		if (demangle)
		{
			int   status;
			char* demangledSymbol = __cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			if (status == 0)
			{
				symbolInfo._function = demangledSymbol;
				free(demangledSymbol);
			}
		}

		return true;
	}
}
