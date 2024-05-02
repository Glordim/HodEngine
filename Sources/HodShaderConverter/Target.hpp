#pragma once

#include <cstdint>

namespace hod
{
    enum class Target : uint8_t
    {
        Vulkan,
        Metal_MacOS,
        Metal_IOS,
        D3D12_Windows,

        Count
    };
}
