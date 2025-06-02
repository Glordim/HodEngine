#pragma once
#include "HodEngine/Core/Export.hpp"

#include <string>

namespace hod
{
	struct SymbolInfo
	{
		std::string _module;
		std::string _function;
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
		static std::string GetLastWin32ErrorMessage();
#endif
	};
}
