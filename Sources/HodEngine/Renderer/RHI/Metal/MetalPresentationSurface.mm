#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalPresentationSurface.hpp"
#include <HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp>

#include <Cocoa/Cocoa.h>

namespace hod::renderer
{
    void MetalPresentationSurface::SetupLayer(window::MacOsWindow* macOsWindow)
    {
        NSView* view = macOsWindow->GetNsView();

        [view setLayer:(__bridge CALayer*)_layer];
        [view setWantsLayer:YES];
    }
}
