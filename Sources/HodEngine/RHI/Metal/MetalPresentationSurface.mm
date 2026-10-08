#include "HodEngine/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/RHI/Pch.hpp"
#include <HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp>

#include <Cocoa/Cocoa.h>

namespace hod::inline rhi
{
	void MetalPresentationSurface::SetupLayer(MacOsWindow *macOsWindow)
	{
		NSView *view = macOsWindow->GetNsView();

		[view setLayer:(__bridge CALayer *)_layer];
		[view setWantsLayer:YES];
		//[view setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawDuringViewResize];
		//[view setLayerContentsPlacement:NSViewLayerContentsPlacementTopLeft];
	}
}
