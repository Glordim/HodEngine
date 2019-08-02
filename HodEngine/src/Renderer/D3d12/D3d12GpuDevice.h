#ifndef __D3D12_GPU_DEVICE_HPP__
#define __D3D12_GPU_DEVICE_HPP__

#include "Renderer/GpuDevice.h"

#include <dxgi1_5.h>

#include <wrl.h>
using namespace Microsoft::WRL;

struct D3d12GpuDevice : public GpuDevice
{
    ComPtr<IDXGIAdapter1> adapter;
};

#endif
