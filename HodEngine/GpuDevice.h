#ifndef __GPU_DEVICE_HPP__
#define __GPU_DEVICE_HPP__

#include <string>

struct GpuDevice
{
    std::wstring name;
    size_t vram;
    size_t score;
    bool compatible;
};

#endif
