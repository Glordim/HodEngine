#pragma once

#include "HodEngine/Renderer/RHI/Device.hpp"

#include <Metal/Metal.hpp>

namespace hod::renderer
{
    class MetalDevice : public Device
    {
    public:

        MetalDevice();
        MTL::Device*  GetNativeDevice() const;

    private:

        MTL::Device*   _device;
    };
}
