#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Debug.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include <cassert>
#include <cstring>

namespace hod
{
	bool Debug::IsDebuggerAttached()
	{
		int mib[4];
		struct kinfo_proc info;
		size_t size = sizeof(info);

		info.kp_proc.p_flag = 0;

		mib[0] = CTL_KERN;
		mib[1] = KERN_PROC;
		mib[2] = KERN_PROC_PID;
		mib[3] = getpid();

		if (sysctl(mib, 4, &info, &size, nullptr, 0) != 0)
			return false;

		return (info.kp_proc.p_flag & P_TRACED) != 0;
	}

	/// @brief
	/// @param callstack
	/// @param maxSize
	/// @return
	uint32_t Debug::GetCallstack(void** callstack, uint32_t maxSize)
	{
		return backtrace(callstack, maxSize);
	}

	/// @brief
	/// @param addr
	/// @return
	String Debug::GetSymbol(void* addr)
	{
		String symbol = "NOT_FOUND";

		char** symbols = backtrace_symbols(&addr, 1);
		if (symbols == nullptr)
		{
			perror("backtrace_symbols");
			exit(EXIT_FAILURE);
		}
		else
		{
			symbol = symbols[0];
			free(symbols);
		}

		return symbol;
	}

	/// @brief
	/// @param addr
	/// @param symbol
	/// @param Size
	/// @return
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
			char* demangledSymbol = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			if (status == 0)
			{
				symbolInfo._function = demangledSymbol;
				free(demangledSymbol);
			}
		}

		return true;
	}
}
