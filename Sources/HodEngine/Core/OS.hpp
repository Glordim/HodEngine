#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod
{
	struct SymbolInfo
	{
		String _module;
		String _function;
		uint64_t	_address;
		uint32_t	_line;
	};

	class HOD_CORE_API OS
	{
	public:

		static uint32_t		GetCallstack(void** callstack, uint32_t maxSize); // todo span ?
		static std::string	GetSymbol(void* addr);
		static bool			GetSymbolInfo(void* addr, SymbolInfo& symbolInfo, bool demangle);

		static bool			OpenFileWithDefaultApp(const char* filePath);

#if defined(PLATFORM_WINDOWS)
		static String GetLastWin32ErrorMessage();
#endif
	};
}
