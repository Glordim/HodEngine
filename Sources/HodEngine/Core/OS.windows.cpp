#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include <win32/dbghelp.h>
#include <win32/misc.h>
#include <win32/process.h>
#include <win32/window.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

extern "C"
{
	typedef enum
	{
		SymNone = 0,
		SymCoff,
		SymCv,
		SymPdb,
		SymExport,
		SymDeferred,
		SymSym, // .sym file
		SymDia,
		SymVirtual,
		NumSymTypes
	} SYM_TYPE;

	typedef struct _IMAGEHLP_MODULE64
	{
		DWORD    SizeOfStruct;         // set to sizeof(IMAGEHLP_MODULE64)
		DWORD64  BaseOfImage;          // base load address of module
		DWORD    ImageSize;            // virtual size of the loaded module
		DWORD    TimeDateStamp;        // date/time stamp from pe header
		DWORD    CheckSum;             // checksum from the pe header
		DWORD    NumSyms;              // number of symbols in the symbol table
		SYM_TYPE SymType;              // type of symbols loaded
		CHAR     ModuleName[32];       // module name
		CHAR     ImageName[256];       // image name
		CHAR     LoadedImageName[256]; // symbol file name
		// new elements: 07-Jun-2002
		CHAR  LoadedPdbName[256];   // pdb file name
		DWORD CVSig;                // Signature of the CV record in the debug directories
		CHAR  CVData[MAX_PATH * 3]; // Contents of the CV record
		DWORD PdbSig;               // Signature of PDB
		GUID  PdbSig70;             // Signature of PDB (VC 7 and up)
		DWORD PdbAge;               // DBI age of pdb
		BOOL  PdbUnmatched;         // loaded an unmatched pdb
		BOOL  DbgUnmatched;         // loaded an unmatched dbg
		BOOL  LineNumbers;          // we have line number information
		BOOL  GlobalSymbols;        // we have internal symbol information
		BOOL  TypeInfo;             // we have type information
		// new elements: 17-Dec-2003
		BOOL SourceIndexed; // pdb supports source server
		BOOL Publics;       // contains public symbols
		// new element: 15-Jul-2009
		DWORD MachineType; // IMAGE_FILE_MACHINE_XXX from ntimage.h and winnt.h
		DWORD Reserved;    // Padding - don't remove.
	} IMAGEHLP_MODULE64, *PIMAGEHLP_MODULE64;

	BOOL      SymGetModuleInfo64(_In_ HANDLE hProcess, _In_ DWORD64 qwAddr, _Out_ PIMAGEHLP_MODULE64 ModuleInfo);
	HINSTANCE ShellExecuteA(_In_opt_ HWND hwnd, _In_opt_ LPCSTR lpOperation, _In_ LPCSTR lpFile, _In_opt_ LPCSTR lpParameters, _In_opt_ LPCSTR lpDirectory, _In_ INT nShowCmd);
	HANDLE    LocalFree(HANDLE hMem);
}

namespace hod
{
	/// @brief
	/// @param callstack
	/// @param maxSize
	/// @return
	uint32_t OS::GetCallstack(void** callstack, uint32_t maxSize)
	{
		return CaptureStackBackTrace(4, maxSize, callstack, nullptr);
	}

	/// @brief
	/// @param addr
	/// @return
	String OS::GetSymbol(void* addr)
	{
		String symbol;
		HANDLE hProcess = GetCurrentProcess();

		static bool symInitialized = false;
		if (symInitialized == false)
		{
			if (SymInitialize(hProcess, NULL, TRUE) == FALSE)
			{
				OUTPUT_ERROR("SymInitialize error : {}", OS::GetLastWin32ErrorMessage());
				return symbol; // assert ?
			}
			symInitialized = true;
		}

		constexpr uint32_t maxFunctionNameSize = 255;
		uint8_t            data[sizeof(SYMBOL_INFO) + maxFunctionNameSize * sizeof(uint8_t)];
		PSYMBOL_INFO       symbolInfo = reinterpret_cast<PSYMBOL_INFO>(data);
		symbolInfo->MaxNameLen = maxFunctionNameSize;
		symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

		if (SymFromAddr(hProcess, reinterpret_cast<DWORD64>(addr), 0, symbolInfo) == TRUE)
		{
			if (symbolInfo->NameLen >= symbolInfo->MaxNameLen)
			{
				symbolInfo->Name[symbolInfo->NameLen - 1] = '\0';
			}
			symbol = symbolInfo->Name;
		}
		else
		{
			symbol = "NOT_FOUND";
		}
		return symbol;
	}

	/// @brief
	/// @param addr
	/// @param symbolInfo
	/// @param demangle
	/// @return
	bool OS::GetSymbolInfo(void* addr, SymbolInfo& symbolInfo, bool demangle)
	{
		HANDLE hProcess = GetCurrentProcess();

		static bool symInitialized = false;
		if (symInitialized == false)
		{
			if (SymInitialize(hProcess, NULL, TRUE) == FALSE)
			{
				OUTPUT_ERROR("SymInitialize error : {}", OS::GetLastWin32ErrorMessage());
				return false;
			}
			symInitialized = true;
		}

		constexpr uint32_t maxFunctionNameSize = 1024;
		uint8_t            data[sizeof(SYMBOL_INFO) + maxFunctionNameSize];
		PSYMBOL_INFO       win32SymbolInfo = reinterpret_cast<PSYMBOL_INFO>(data);
		win32SymbolInfo->MaxNameLen = maxFunctionNameSize;
		win32SymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

		DWORD64 displacement = 0;
		if (SymFromAddr(hProcess, reinterpret_cast<DWORD64>(addr), &displacement, win32SymbolInfo) == FALSE)
		{
			return false;
		}

		if (win32SymbolInfo->NameLen >= win32SymbolInfo->MaxNameLen)
		{
			win32SymbolInfo->Name[win32SymbolInfo->NameLen - 1] = '\0';
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(moduleInfo));
		moduleInfo.SizeOfStruct = sizeof(moduleInfo);

		if (SymGetModuleInfo64(hProcess, win32SymbolInfo->ModBase, &moduleInfo) == FALSE)
		{
			symbolInfo._module = "???";
		}
		else
		{
			Path modulePath(moduleInfo.ImageName);
			symbolInfo._module = modulePath.Filename().GetString();
		}

		symbolInfo._address = reinterpret_cast<void*>(win32SymbolInfo->Address);
		symbolInfo._function = win32SymbolInfo->Name;
		symbolInfo._line = static_cast<uint32_t>(displacement);

		if (demangle)
		{
		}

		return true;
	}

	/// @brief
	/// @return
	String OS::GetLastWin32ErrorMessage()
	{
		LPVOID lpMsgBuf;
		DWORD  dw = GetLastError();

		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		                 (LPTSTR)&lpMsgBuf, 0, NULL);

		String message = (char*)lpMsgBuf;

		::LocalFree(lpMsgBuf);

		return message;
	}

	/// @brief
	/// @param filePath
	bool OS::OpenFileWithDefaultApp(const char* filePath)
	{
		HINSTANCE result = ShellExecuteA(NULL, "open", filePath, NULL, NULL, SW_SHOWNORMAL);
		if ((INT_PTR)result <= 32)
		{
			OUTPUT_ERROR("OS::OpenFileWithDefaultApp: ShellExecute open {} fail ({})", filePath, OS::GetLastWin32ErrorMessage());
			return false;
		}
		return true;
	}
}
