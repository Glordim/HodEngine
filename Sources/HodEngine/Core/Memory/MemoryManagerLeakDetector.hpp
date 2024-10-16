#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/Config.hpp"

#if defined(HOD_ENABLED_MEMLEAK_DETECTOR)

#include <cstdint>
#include <unordered_map>
#include <array>
#include <mutex>

namespace hod
{
    class HOD_CORE_API MemoryManagerLeakDetector
    {
    public:

        static MemoryManagerLeakDetector _instance;
        static MemoryManagerLeakDetector* GetInstance() { return &_instance; }

    public:

        struct Allocation
        {
            void* _realAddress = nullptr;
            void* _userAddress = nullptr;
            uint32_t _size = 0;
            uint32_t _index = 0;

            std::array<void*, 64> _callstack;
            uint32_t _callstackSize = 0;
        };

    public:

        ~MemoryManagerLeakDetector();

        void*    Allocate(uint32_t size);
        void     Free(void* ptr);

        void*    AllocateAlign(uint32_t size, uint32_t alignment);
        void     FreeAlign(void* ptr, uint32_t alignment);

    private:

        void        InsertAllocation(void* userAddress, uint32_t size, void* address);
        void*       RemoveAllocation(void* userAddress);

    private:

        std::mutex _mutex;
        std::array<Allocation*, 1024 * 32> _allocations; // todo boost or use vector with custom allocator
        uint32_t _allocationCount = 0;
    };
}

#endif
