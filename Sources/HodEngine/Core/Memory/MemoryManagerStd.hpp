#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/Config.hpp"

#if !defined(HOD_ENABLED_MEMLEAK_DETECTOR)

namespace hod
{
    class HOD_CORE_API MemoryManagerStd
    {
    public:

        static MemoryManagerStd _instance;
        static MemoryManagerStd* GetInstance() { return &_instance; }

    public:

        void*    Allocate(uint32_t size);
        void     Free(void* ptr);

        void*    AllocateAlign(uint32_t size, uint32_t alignment);
        void     FreeAlign(void* ptr, uint32_t alignment);
    };
}

#endif
