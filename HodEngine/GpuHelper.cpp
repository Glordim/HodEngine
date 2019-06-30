#include "GpuHelper.h"

#include "Renderer.h"

bool GpuHelper::GetAvailableDevices(std::vector<GpuHelper::Device>* availableDevices)
{
    if (availableDevices == nullptr)
        return false;

    Renderer* renderer = Renderer::GetInstance();

    if (renderer == nullptr)
        return false;

    return renderer->GetPhysicalDeviceList(availableDevices);
}

bool GpuHelper::GetBestAvailableAndCompatibleDevice(GpuHelper::Device* bestDevice)
{
    if (bestDevice == nullptr)
        return false;

    Renderer* renderer = Renderer::GetInstance();

    if (renderer == nullptr)
        return false;

    std::vector<GpuHelper::Device> availableDevices;
    
    if (renderer->GetPhysicalDeviceList(&availableDevices) == false)
        return false;

    size_t deviceCount = availableDevices.size();
    for (int i = 0; i < deviceCount; ++i)
    {
        GpuHelper::Device& device = availableDevices[i];

        if (device.compatible == false)
            continue;

        if (bestDevice == nullptr || device.score > bestDevice->score)
            *bestDevice = device;
    }

    if (bestDevice == nullptr)
        return false;

    return true;
}
