#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Memory/MemoryManagerLeakDetector.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)
#include <unwind.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <cassert>

namespace hod
{
	struct BacktraceState
	{
		uint32_t _currentIndex = 0;
		std::array<void*, 64>& _callstack;
	};

	static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
	{
		BacktraceState* state = static_cast<BacktraceState*>(arg);
		uintptr_t pc = _Unwind_GetIP(context);
		if (pc)
		{
			if (state->_currentIndex == state->_callstack.size())
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
	}

	/// @brief 
	/// @param report 
	/// @param allocation 
	void MemoryManagerLeakDetector::WriteCallstackInReport(FILE* report, const Allocation& allocation) const
	{
		for (uint32_t currentFrame = 0; currentFrame < allocation._callstackSize; ++currentFrame)
		{
			const char* symbol = "NOT_FOUND";

			Dl_info info;
			if (dladdr(allocation._callstack[currentFrame], &info) && info.dli_sname)
			{
				symbol = info.dli_sname;
			}

			fprintf(report, "\t%s\n", symbol);
		}
	}

    /// @brief 
    /// @param callstack 
    /// @return 
    uint32_t MemoryManagerLeakDetector::GetCallstack(std::array<void*, 64>& callstack) const
	{
		BacktraceState state = { 0, callstack };
    	_Unwind_Backtrace(unwindCallback, &state);
		return state._currentIndex;
	}
}

#endif
