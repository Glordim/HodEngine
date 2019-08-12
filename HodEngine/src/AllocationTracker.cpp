//
//  AllocationTracker.cpp
//  BO
//
//  Created by CyrilDarkVador on 11/02/2017.
//
//

#if defined(_WIN32) && defined(_DEBUG)

#include "AllocationTracker.hpp"

extern char WindowsPathAtStart[];

const char AllocationTracker::filenameOutput[] = "MemoryLeak.txt";
const char AllocationTracker::stopStackTraceDumpStackSymbol[] = "cocos2d::Application::run";

size_t AllocationTracker::allocationSizeContainer[] = { 512,256,128,64,32,16,8,0 };

//**************************************************************************************************************************************
//**************************************************************************************************************************************
//Copie de dbgint.h
#define nNoMansLandSize 4
typedef struct _CrtMemBlockHeader
{
	struct _CrtMemBlockHeader * pBlockHeaderNext;
	struct _CrtMemBlockHeader * pBlockHeaderPrev;
	char *                      szFileName;
	int                         nLine;
#ifdef _WIN64
	/* These items are reversed on Win64 to eliminate gaps in the struct
	* and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
	* maintained in the debug heap.
	*/
	int                         nBlockUse;
	size_t                      nDataSize;
#else  /* _WIN64 */
	size_t                      nDataSize;
	int                         nBlockUse;
#endif  /* _WIN64 */
	long                        lRequest;
	unsigned char               gap[nNoMansLandSize];
	/* followed by:
	*  unsigned char           data[nDataSize];
	*  unsigned char           anotherGap[nNoMansLandSize];
	*/
} _CrtMemBlockHeader;

//**************************************************************************************************************************************
//**************************************************************************************************************************************
_CRT_ALLOC_HOOK AllocationTracker::oldAllocHook;

long AllocationTracker::allocationFirstRequest = -1;
size_t AllocationTracker::allocationNbContainer;
size_t* AllocationTracker::allocationNbUsed;
size_t* AllocationTracker::allocationNbAllocMade;
std::vector<AllocationTracker_InfoElement>* AllocationTracker::allocationLogs;

FILE* AllocationTracker::DumpTracking_FILE;
char* AllocationTracker::DumpTracking_FILE_BufferBase;
const size_t AllocationTracker::DumpTracking_FILE_BufferSize = 128*1024;
size_t AllocationTracker::DumpTracking_FILE_BufferPos;
size_t AllocationTracker::DumpTracking_FILE_BufferNbInside;

HANDLE AllocationTracker::g_MemoryHeap = 0;
long AllocationTracker::fastIndexCurrentSize = 0;
std::vector<size_t*> AllocationTracker::fastIndexBlock;

AllocationTracker::RtlAllocateHeap_t AllocationTracker::RtlAllocateHeap;
AllocationTracker::RtlFreeHeap_t AllocationTracker::RtlFreeHeap;

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void AllocationTracker::Start_Tracking()
{
	allocationNbContainer = sizeof(allocationSizeContainer)/sizeof(allocationSizeContainer[0]);

	allocationNbUsed = new size_t[allocationNbContainer];
	allocationNbAllocMade = new size_t[allocationNbContainer];
	allocationLogs = new std::vector<AllocationTracker_InfoElement>[allocationNbContainer];
	for (size_t index = 0; index < allocationNbContainer; index++)
	{
		allocationNbUsed[index] = 0;
		allocationNbAllocMade[index] = 0;
		allocationLogs[index].clear();
	}

	g_MemoryHeap = HeapCreate(0x0, 0, 0);
	HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
	if (ntdll)
	{
		RtlAllocateHeap = (RtlAllocateHeap_t)GetProcAddress(ntdll, "RtlAllocateHeap");
		RtlFreeHeap = (RtlFreeHeap_t)GetProcAddress(ntdll, "RtlFreeHeap");
	}

	oldAllocHook = _CrtSetAllocHook(DoAllocationLogging);
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void AllocationTracker::End_Tracking_And_Dump()
{
	_CrtSetAllocHook(oldAllocHook);

	OutputDebugStringA("LOG MEMORY LEAK TO FILE\n");

	char locPathForOutput[1024];
	strcpy(locPathForOutput, WindowsPathAtStart);
	strcat(locPathForOutput, "\\");
	strcat(locPathForOutput, filenameOutput);

	DumpTracking_OpenFile(locPathForOutput);

	char szBuff[1024];

	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);

	SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + (maxFunctionNameLength + 1) * sizeof(TCHAR));
	symbol->MaxNameLen = maxFunctionNameLength;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	DWORD displacement;
	IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
	line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	OutputDebugStringA("LOG MEMORY LEAK TO FILE - SORTING\n");
	long locTotalNbAllocMade = 0;
	long locTotalNbAllocLeaked = 0;
	long locTotalSizeLeaked = 0;
	std::vector< std::pair<long, AllocationTracker_InfoElement*> > listOfLeakToSort;
	for (size_t locContainer = 0; locContainer < allocationNbContainer; locContainer++)
	{
		locTotalNbAllocMade += allocationNbAllocMade[locContainer];
		size_t locIndex = 0;
		while (locIndex < allocationNbUsed[locContainer])
		{
			locTotalNbAllocLeaked++;
			locTotalSizeLeaked += allocationLogs[locContainer][locIndex].size;
			listOfLeakToSort.push_back(std::make_pair(allocationLogs[locContainer][locIndex].requestNumber, &allocationLogs[locContainer][locIndex]));
			locIndex++;
		}
	}
	std::sort(listOfLeakToSort.begin(), listOfLeakToSort.end(), SortLeak);
	OutputDebugStringA("LOG MEMORY LEAK TO FILE - SORTING DONE\n");

	DumpTracking_WriteStringToFile("Leak Detector V3.0 - (c)gumiEurope - Designed by CyrilDarkVador\n");
	DumpTracking_WriteStringFormatToFile("Number of alloc made : %d\n", locTotalNbAllocMade);
	DumpTracking_WriteStringFormatToFile("Number of alloc leaked : %d\n", locTotalNbAllocLeaked);
	DumpTracking_WriteStringFormatToFile("Total memory leaked : %d bytes\n", locTotalSizeLeaked);
	DumpTracking_WriteStringToFile("\n");

	for (size_t locContainer = 0; locContainer < allocationNbContainer; locContainer++)
	{
		DumpTracking_WriteStringFormatToFile("Number of alloc made of size > %d : %d\n", allocationSizeContainer[locContainer],allocationNbAllocMade[locContainer]);
	}
	DumpTracking_WriteStringToFile("\n");


	std::map<DWORD64, char*> listOfResolvedSymbol;

	size_t locIndex = 0;
	int lastPercent = -1;
	while (locIndex < listOfLeakToSort.size())
	{
		int currentPercent = (int)((locIndex * 100) / listOfLeakToSort.size());
		if (currentPercent != lastPercent)
		{
			_snprintf(szBuff, sizeof(szBuff), "LOG MEMORY LEAK TO FILE %d %% (%d / %d)\n", currentPercent, locIndex, listOfLeakToSort.size());
			OutputDebugStringA(szBuff);
			lastPercent = currentPercent;
		}

		AllocationTracker_InfoElement& currentLeak = *listOfLeakToSort[locIndex].second;

		DWORD64 adressBreakDisplayStack = 0;

		DumpTracking_WriteStringFormatToFile("---------- Request %d : %d bytes ----------\n", currentLeak.requestNumber, currentLeak.size);

		if (currentLeak.numberOfFrames == maxStackFrames)
			DumpTracking_WriteStringToFile("\t\tSTACK FRAMES TOO BIG\n");

		for (int i = 0; i <currentLeak.numberOfFrames; i++)
		{
			DWORD64 address = (DWORD64)(currentLeak.stackTraceInfo[i]);
			if (address == adressBreakDisplayStack)
				break;

			char* lineToPrint = listOfResolvedSymbol[address];
			if (lineToPrint == nullptr)
			{
				SymFromAddr(process, address, NULL, symbol);
				if (SymGetLineFromAddr64(process, address, &displacement, line))
				{
					if ((adressBreakDisplayStack == 0) && (strcmp(symbol->Name, stopStackTraceDumpStackSymbol) == 0))
					{
						adressBreakDisplayStack = address;
						break;
					}
					char* newLineForAdress = new char[maxFunctionNameLength];
					_snprintf(newLineForAdress, maxFunctionNameLength-1, "\t\t%s (%lu): %s (address: 0x%I64X)\n", line->FileName, line->LineNumber, symbol->Name, symbol->Address);
					listOfResolvedSymbol[address] = newLineForAdress;
					lineToPrint = newLineForAdress;
				}
				else
				{
					if ((adressBreakDisplayStack == 0) && (strcmp(symbol->Name, stopStackTraceDumpStackSymbol) == 0))
					{
						adressBreakDisplayStack = address;
						break;
					}
					char* newLineForAdress = new char[maxFunctionNameLength];
					_snprintf(newLineForAdress, maxFunctionNameLength-1, "\t\t%s (address: 0x%I64X)\n", symbol->Name, symbol->Address);
					listOfResolvedSymbol[address] = newLineForAdress;
					lineToPrint = newLineForAdress;
				}
			}
			DumpTracking_WriteStringToFile(lineToPrint);

		}
		DumpTracking_WriteStringToFile("\n");

		locIndex++;
	}

	DumpTracking_CloseFile();

	OutputDebugStringA("LOG MEMORY LEAK TO FILE DONE \n");

	delete[] allocationNbAllocMade;
	delete[] allocationNbUsed;
	delete[] allocationLogs;
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
int AllocationTracker::DoAllocationLogging(	int allocType,
											void *userData,
											size_t size,
											int blockType,
											long requestNumber,
											const unsigned char *filename,
											int lineNumber)
{
	//if (blockType == _CRT_BLOCK)
	//	return TRUE;

	switch (allocType)
	{
		case _HOOK_ALLOC:
		{
			if (allocationFirstRequest == -1)
				allocationFirstRequest = requestNumber;
			DoLogAlloc(size, requestNumber);

			break;
		}
		case _HOOK_REALLOC:
		{
			const _CrtMemBlockHeader* header = ((const _CrtMemBlockHeader*)userData) - 1;

			DoUnlogAlloc(header->nBlockUse, header->lRequest);

			DoLogAlloc(size, requestNumber);

			break;
		}
		case _HOOK_FREE:
		{
			const _CrtMemBlockHeader* header = ((const _CrtMemBlockHeader*)userData) - 1;
			long locRequestNumber = header->lRequest;

			size = header->nBlockUse;

			DoUnlogAlloc(size, locRequestNumber);

			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************

void AllocationTracker::DoLogAlloc(size_t size, long requestNumber)
{
	int locContainer = 0;
	for (auto sizeContainer : allocationSizeContainer)
	{
		if (size >= sizeContainer)
			break;
		locContainer++;
	}

	long requestNumberIndex = requestNumber - allocationFirstRequest;
	size_t indexBlock = requestNumberIndex / fastIndexGrowingSize;
	size_t indexBlockNum = requestNumberIndex - (indexBlock*fastIndexGrowingSize);
	if (requestNumberIndex >= fastIndexCurrentSize)
	{
		_CrtSetAllocHook(oldAllocHook);

		size_t* locNewFastIndex = (size_t*)RtlAllocateHeap(g_MemoryHeap, 0x0, sizeof(size_t) * (fastIndexGrowingSize));

		fastIndexBlock.push_back(locNewFastIndex);

		_CrtSetAllocHook(DoAllocationLogging);

		fastIndexCurrentSize += fastIndexGrowingSize;
	}

	if (allocationLogs[locContainer].size() == allocationNbUsed[locContainer])
	{
		_CrtSetAllocHook(oldAllocHook);

		allocationLogs[locContainer].resize(allocationLogs[locContainer].size() + 16384);

		_CrtSetAllocHook(DoAllocationLogging);
	}

	size_t locIndexForLog = allocationNbUsed[locContainer];

	AllocationTracker_InfoElement& locInfoToUse = allocationLogs[locContainer][locIndexForLog];

	WORD numberOfFrames = CaptureStackBackTrace(2, maxStackFrames, locInfoToUse.stackTraceInfo, NULL);

	locInfoToUse.size = size;
	locInfoToUse.requestNumber = requestNumber;
	locInfoToUse.numberOfFrames = numberOfFrames;

	allocationNbUsed[locContainer]++;
	allocationNbAllocMade[locContainer]++;
	size_t* locBlock = fastIndexBlock[indexBlock];
	locBlock[indexBlockNum] = locIndexForLog;
}


//**************************************************************************************************************************************
//**************************************************************************************************************************************
void AllocationTracker::DoUnlogAlloc(size_t size, long requestNumber)
{
	if (requestNumber < allocationFirstRequest)
		return;

	int locContainer = 0;
	for (auto sizeContainer : allocationSizeContainer)
	{
		if (size >= sizeContainer)
			break;
		locContainer++;
	}

	long requestNumberIndex = requestNumber - allocationFirstRequest;
	size_t indexBlock = requestNumberIndex / fastIndexGrowingSize;
	size_t indexBlockNum = requestNumberIndex - (indexBlock*fastIndexGrowingSize);
	size_t* locBlock = fastIndexBlock[indexBlock];

	if (allocationNbUsed[locContainer] > 1)
	{
		AllocationTracker_InfoElement* locElementPtr = &allocationLogs[locContainer][locBlock[indexBlockNum]];
		
		AllocationTracker_InfoElement* locElementPtr2 = &allocationLogs[locContainer][allocationNbUsed[locContainer] - 1];
		long requestNumberIndex2 = locElementPtr2->requestNumber - allocationFirstRequest;
		size_t indexBlock2 = requestNumberIndex2 / fastIndexGrowingSize;
		size_t indexBlockNum2 = requestNumberIndex2 - (indexBlock2*fastIndexGrowingSize);
		size_t* locBlock2 = fastIndexBlock[indexBlock2];

		*locElementPtr = *locElementPtr2;

		locBlock2[indexBlockNum2] = locBlock[indexBlockNum];
	}
	allocationNbUsed[locContainer]--;
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
bool AllocationTracker::SortLeak(std::pair<long, AllocationTracker_InfoElement*> elementA, std::pair<long, AllocationTracker_InfoElement*> elementB)
{
	return (elementA.first <= elementB.first);
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void AllocationTracker::DumpTracking_OpenFile(const char* filename)
{
	DumpTracking_FILE = fopen(filename, "w");

	DumpTracking_FILE_BufferBase = new char[DumpTracking_FILE_BufferSize];
	DumpTracking_FILE_BufferPos = 0;
	DumpTracking_FILE_BufferNbInside = 0;
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void AllocationTracker::DumpTracking_WriteStringToFile(const char* string)
{
	size_t lenOfString = strlen(string);
	if (DumpTracking_FILE_BufferNbInside + lenOfString > DumpTracking_FILE_BufferSize)
	{
		fwrite(DumpTracking_FILE_BufferBase, 1, DumpTracking_FILE_BufferNbInside, DumpTracking_FILE);
		DumpTracking_FILE_BufferPos = 0;
		DumpTracking_FILE_BufferNbInside = 0;
	}

	memcpy(DumpTracking_FILE_BufferBase + DumpTracking_FILE_BufferPos, string, lenOfString);

	DumpTracking_FILE_BufferPos += lenOfString;
	DumpTracking_FILE_BufferNbInside += lenOfString;
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void AllocationTracker::DumpTracking_WriteStringFormatToFile(const char* format, ...)
{
	char stringToWrite[maxFunctionNameLength*2];
	va_list args;
	va_start(args, format);
	_vsnprintf(stringToWrite, (maxFunctionNameLength*2)-1, format, args);
	va_end(args);
	DumpTracking_WriteStringToFile(stringToWrite);
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void AllocationTracker::DumpTracking_CloseFile()
{
	fwrite(DumpTracking_FILE_BufferBase, 1, DumpTracking_FILE_BufferNbInside, DumpTracking_FILE);
	fclose(DumpTracking_FILE);
}

#endif	//#if defined(_WIN32) && defined(_DEBUG)