#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cstdint>
#include <unordered_map>
#include <array>
#include <mutex>

namespace hod
{
    class MemoryManager
    {
        static MemoryManager _instance;

    public:

        struct Allocation
        {
            void* _ptr = nullptr;
            uint32_t _size = 0;

            std::array<void*, 64> _callstack;
            uint32_t _callstackSize = 0;
        };

    public:

        MemoryManager();
        ~MemoryManager();

    public:

        static void*    Allocate(uint32_t size);
        static void     Free(void* ptr);

        static void*    AllocateAlign(uint32_t size, uint32_t alignment);
        static void     FreeAlign(void* ptr, uint32_t alignment);

    private:

        std::mutex _mutex;
        std::array<Allocation, 1024 * 1024> _allocations;
        uint32_t _allocationCount = 0;
    };
}
