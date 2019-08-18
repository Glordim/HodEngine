//
//  AllocationTracker.hpp
//  BO
//
//  Created by CyrilDarkVador on 11/02/2017.
//
//

#ifndef AllocationTracker_hpp
#define AllocationTracker_hpp

#include "stdio.h"
#include <Windows.h>

#pragma warning( push )
#pragma warning( disable : 4091 )	// typedef inside dbghelp.h generate a warning C4091
#include <dbghelp.h>
#pragma warning( pop )

#include <fstream>

class AllocationTracker_InfoElement;

class AllocationTracker
{
public:
	static void Start_Tracking();
	static void End_Tracking_And_Dump();

	static const int maxStackFrames = 32;
	static const int maxFunctionNameLength = 1024;

	static const size_t fastIndexGrowingSize = 1000000;

private:
	typedef LPVOID(__stdcall *RtlAllocateHeap_t) (HANDLE, DWORD, SIZE_T);
	typedef BYTE(__stdcall *RtlFreeHeap_t) (HANDLE, DWORD, LPVOID);
	static RtlAllocateHeap_t   RtlAllocateHeap;
	static RtlFreeHeap_t       RtlFreeHeap;

	static const char filenameOutput[];
	static const char stopStackTraceDumpStackSymbol[];
	static size_t allocationSizeContainer[];

	static _CRT_ALLOC_HOOK oldAllocHook;
	static long allocationFirstRequest;
	static size_t allocationNbContainer;
	static size_t* allocationNbUsed;
	static size_t* allocationNbAllocMade;
	static std::vector<AllocationTracker_InfoElement>* allocationLogs;

	static HANDLE g_MemoryHeap;
	static long fastIndexCurrentSize;
	static std::vector<size_t*> fastIndexBlock;

	static std::ofstream DumpTracking_FILE;
	static char* DumpTracking_FILE_BufferBase;
	static const size_t DumpTracking_FILE_BufferSize;
	static size_t DumpTracking_FILE_BufferPos;
	static size_t DumpTracking_FILE_BufferNbInside;

	static int DoAllocationLogging(int allocType,
		void *userData,
		size_t size,
		int blockType,
		long requestNumber,
		const unsigned char *filename,
		int lineNumber);

	static void DoLogAlloc(size_t size, long requestNumber);
	static void DoUnlogAlloc(size_t size, long requestNumber);

	static bool SortLeak(std::pair<long, AllocationTracker_InfoElement*> elementA, std::pair<long, AllocationTracker_InfoElement*> elementB);
	static void DumpTracking_OpenFile(const char* filename);
	static void DumpTracking_WriteStringToFile(const char* string);
	static void DumpTracking_WriteStringFormatToFile(const char* format, ...);
	static void DumpTracking_CloseFile();

};

class AllocationTracker_InfoElement
{
public:
	AllocationTracker_InfoElement()
	{
	}

	size_t size;
	long requestNumber;
	WORD numberOfFrames;
	void* stackTraceInfo[AllocationTracker::maxStackFrames];
};

#endif /* AllocationTracker_hpp */
