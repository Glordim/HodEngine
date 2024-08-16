#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"
#include <HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp>

#include <Cocoa/Cocoa.h>

namespace hod::renderer
{
    void MetalContext::SetupLayer(window::MacOsWindow* macOsWindow)
    {
        NSView* view = macOsWindow->GetNsView();

        [view setLayer:(CALayer*)_layer];
        [view setWantsLayer:YES];
    }
}
