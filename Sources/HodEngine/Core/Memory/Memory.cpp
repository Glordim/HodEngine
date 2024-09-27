#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Memory/Memory.hpp"

#include <cstdlib>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>

namespace hod
{
    #pragma warning(disable:4075)
    #pragma init_seg(".CRT$XCA-000")
    MemoryManager MemoryManager::_instance;
    #pragma warning(default:4075)

	/// @brief 
	MemoryManager::MemoryManager()
	{
		//_allocations.reserve(1024);
	}

    /// @brief 
    MemoryManager::~MemoryManager()
	{
		//_allocations.clear();

		uint32_t memleakCount = _instance._allocationCount;
		if (memleakCount > 0)
		{
			HANDLE hProcess = GetCurrentProcess();
			SymInitialize(hProcess, NULL, TRUE);

			FILE* memleakReport = fopen("MemleakReport.txt", "w");
			fprintf(memleakReport, "Memleak count = %u\n\n", memleakCount);

			for (uint32_t index = 0; index < memleakCount; ++index)
			{
				const Allocation& allocation = _instance._allocations[index];

				fprintf(memleakReport, "Ptr = %p\n", allocation._ptr);
				fprintf(memleakReport, "Size = %u\n", allocation._size);
				fprintf(memleakReport, "Callstack :\n");

				constexpr uint32_t maxFunctionNameSize = 255;
				uint8_t data[sizeof(SYMBOL_INFO) + maxFunctionNameSize * sizeof(uint8_t)];
				PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(data);
				symbol->MaxNameLen = maxFunctionNameSize;
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

				for (uint32_t currentFrame = 0; currentFrame < allocation._callstackSize; ++currentFrame)
				{
					if (SymFromAddr(hProcess, reinterpret_cast<DWORD64>(allocation._callstack[currentFrame]), 0, symbol) == TRUE)
					{
						if (symbol->NameLen == symbol->MaxNameLen)
						{
							symbol->Name[symbol->NameLen - 1] = '\0';
						}
						fprintf(memleakReport, "\t%s\n", symbol->Name);
					}
					else
					{
						fprintf(memleakReport, "\tNOT_FOUND\n");
					}
				}
				fprintf(memleakReport, "\n\n");
			}
			fclose(memleakReport);
		}
	}

    /// @brief 
    /// @param size 
    /// @return 
    void* MemoryManager::Allocate(uint32_t size)
    {
        Allocation allocation;
        allocation._callstackSize = CaptureStackBackTrace(0, 64, allocation._callstack.data(), nullptr);
        allocation._size = size;
        allocation._ptr = malloc(size);

		_instance._mutex.lock();
        _instance._allocations[_instance._allocationCount] = allocation;
		++_instance._allocationCount;
		_instance._mutex.unlock();
        return allocation._ptr;
    }

	/// @brief 
    /// @param size 
    /// @param alignment 
    /// @return 
    void* MemoryManager::AllocateAlign(uint32_t size, uint32_t alignment)
    {
        Allocation allocation;
        allocation._callstackSize = CaptureStackBackTrace(0, 64, allocation._callstack.data(), nullptr);
        allocation._size = size;
        allocation._ptr = _aligned_malloc(size, alignment);

		_instance._mutex.lock();
        _instance._allocations[_instance._allocationCount] = allocation;
		++_instance._allocationCount;
		_instance._mutex.unlock();
        return allocation._ptr;
    }

    /// @brief 
    /// @param ptr 
    void MemoryManager::Free(void* ptr)
    {
		_instance._mutex.lock();
		for (uint32_t index = 0; index < _instance._allocationCount; ++index)
		{
			if (_instance._allocations[index]._ptr == ptr)
			{
				if (index != _instance._allocationCount - 1)
				{
					std::swap(_instance._allocations[index], _instance._allocations[_instance._allocationCount - 1]);
				}
				--_instance._allocationCount;
				break;
			}
		}
		_instance._mutex.unlock();
		free(ptr);
    }

	/// @brief 
    /// @param ptr 
    void MemoryManager::FreeAlign(void* ptr, uint32_t alignment)
    {
		_instance._mutex.lock();
		for (uint32_t index = 0; index < _instance._allocationCount; ++index)
		{
			if (_instance._allocations[index]._ptr == ptr)
			{
				if (index != _instance._allocationCount - 1)
				{
					std::swap(_instance._allocations[index], _instance._allocations[_instance._allocationCount - 1]);
				}
				--_instance._allocationCount;
				break;
			}
		}
		_instance._mutex.unlock();
		_aligned_free(ptr);
    }
}

/// @brief 
/// @param count 
/// @return 
void* operator new(std::size_t count)
{
	return hod::MemoryManager::Allocate(static_cast<uint32_t>(count));
}

/// @brief 
/// @param count 
/// @param al 
/// @return 
void* operator new(std::size_t count, std::align_val_t alignment)
{
	return hod::MemoryManager::AllocateAlign(static_cast<uint32_t>(count), static_cast<uint32_t>(alignment));
}

/// @brief 
/// @param ptr 
void operator delete(void* ptr) noexcept
{
	hod::MemoryManager::Free(ptr);
}

/// @brief 
/// @param ptr 
/// @param  
void operator delete(void* ptr, std::align_val_t alignment) noexcept
{
	hod::MemoryManager::FreeAlign(ptr, static_cast<uint32_t>(alignment));
}
