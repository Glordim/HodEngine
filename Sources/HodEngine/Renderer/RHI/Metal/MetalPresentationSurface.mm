#include "HodEngine/Renderer/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/Renderer/Pch.hpp"
#include <HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp>

#include <Cocoa/Cocoa.h>

namespace hod::inline renderer {
void MetalPresentationSurface::SetupLayer(MacOsWindow *macOsWindow) {
  NSView *view = macOsWindow->GetNsView();

  [view setLayer:(__bridge CALayer *)_layer];
  [view setWantsLayer:YES];
}
} // namespace hod::inline renderer
