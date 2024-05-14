#include "HodEngine/Renderer/RHI/Metal/MetalDevice.hpp"

namespace hod::renderer
{
    MetalDevice::MetalDevice()
    {
        _device = MTL::CreateSystemDefaultDevice();
    }
}
