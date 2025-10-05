#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unwind.h>

#include <cassert>

namespace hod
{
	/// @brief
	/// @param callstack
	/// @param maxSize
	/// @return
	uint32_t OS::GetCallstack(void** callstack, uint32_t maxSize)
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
	String OS::GetSymbol(void* addr)
	{
		String symbol = "NOT_FOUND";

		Dl_info info;
		if (dladdr(addr, &info) && info.dli_sname)
		{
			symbol = info.dli_sname;
		}
		return symbol;
	}
}
