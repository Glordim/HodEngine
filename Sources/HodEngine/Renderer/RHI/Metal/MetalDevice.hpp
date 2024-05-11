#pragma once

#include "HodEngine/Renderer/RHI/Device.hpp"

#include <Metal/Metal.h>

namespace hod::renderer
{
    class MetalDevice : public Device
    {
    public:

        MetalDevice();
        id<MTLDevice>  GetNativeDevice() const;

    private:

        id<MTLDevice>   _device;
    };
}
