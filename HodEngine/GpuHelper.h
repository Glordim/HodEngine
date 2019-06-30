#ifndef __GPU_HELPER_HPP__
#define __GPU_HELPER_HPP__

#include <string>
#include <vector>

class GpuHelper
{
public:

    struct Device
    {
        std::string name;
        int score;
        bool compatible;
        void* handle;
        uint32_t handleQueueIndex;
    };

    static bool GetAvailableDevices(std::vector<Device>* availableDevices);
    static bool GetBestAvailableAndCompatibleDevice(Device* bestDevice);
};

#endif
