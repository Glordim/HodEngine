#pragma once

#include <vector>

#include "GpuDevice.h"

namespace HOD
{
    class GpuDeviceHelper
    {
    public:
        static bool GetAvailableDevices(std::vector<GpuDevice*>* availableDevices);
        static bool GetBestAvailableAndCompatibleDevice(GpuDevice** bestDevice);
    };
}
