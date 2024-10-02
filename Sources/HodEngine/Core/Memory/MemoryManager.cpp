#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Memory/MemoryManager.hpp"

namespace hod
{
    #pragma warning(disable:4075)
    #pragma init_seg(".CRT$XCA-000")
    MemoryManager MemoryManager::_instance;
    #pragma warning(default:4075)
}
