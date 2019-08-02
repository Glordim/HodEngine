#ifndef __GPU_DEVICE_HELPER_HPP__
#define __GPU_DEVICE_HELPER_HPP__

#include <vector>

#include "GpuDevice.h"

class GpuDeviceHelper
{
public:
    static bool GetAvailableDevices(std::vector<GpuDevice*>* availableDevices);
    static bool GetBestAvailableAndCompatibleDevice(GpuDevice** bestDevice);
};

#endif
