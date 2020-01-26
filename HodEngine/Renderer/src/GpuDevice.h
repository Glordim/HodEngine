#pragma once

#include <string>

namespace HOD
{
    struct GpuDevice
    {
        std::wstring name;
        size_t vram;
        size_t score;
        bool compatible;
    };
}
