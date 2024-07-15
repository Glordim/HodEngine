#include "MacOsWindow.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"

#include <Cocoa/Cocoa.h>

	@interface CustomView : NSView
	@end

	@implementation CustomView

	- (void)drawRect:(NSRect)dirtyRect {
		[[NSColor whiteColor] setFill];
		NSRectFill(dirtyRect);
		[super drawRect:dirtyRect];
	}

	@end

namespace hod::window
{
	/// @brief 
	MacOsWindow::MacOsWindow()
		: DesktopWindow()
	{
		NSRect frame = NSMakeRect(0, 0, _width, _height);
        _window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

        [_window setTitle:@"Window"];

        CustomView *customView = [[CustomView alloc] initWithFrame:frame];
		_view = customView;
		_view.wantsLayer = YES;

        //[_window.contentView addSubview:customView];
		[_window setContentView:customView];

        [_window makeKeyAndOrderFront:nil];
	}

	/// @brief 
	MacOsWindow::~MacOsWindow()
	{
		
	}

	/// @brief 
	void MacOsWindow::SetupLayer()
	{
		renderer::MetalContext* metalContext = static_cast<renderer::MetalContext*>(GetGraphicsContext());
		[_view setLayer:(CALayer*)metalContext->GetLayer()];
        [_view setWantsLayer:YES];
		//[_view.layer setNeedsDisplay];
	}

	void MacOsWindow::EventLoop()
	{
        @autoreleasepool 
        {
            while (true)
            {
                NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                    untilDate:[NSDate distantPast]
                                                       inMode:NSDefaultRunLoopMode
                                                      dequeue:YES];
                if (event == nil)
                    break;

                [NSApp sendEvent:event];
            }
        } // autoreleasepool
	}

	/// @brief 
	/// @param width 
	/// @param height 
	void MacOsWindow::SetSize(uint16_t width, uint16_t height)
	{
		_width = width;
		_height = height;

		NSSize newSize = NSMakeSize(_width, _height);
        [_window setContentSize:newSize];
        
        renderer::MetalContext* metalContext = static_cast<renderer::MetalContext*>(GetGraphicsContext());
        metalContext->Resize(width, height);
	}

	/// @brief 
	void MacOsWindow::CenterToScreen()
	{
		//_nativeWinwdow.Center();
		[_window center];
	}

	/// @brief 
	void MacOsWindow::Maximize()
	{
		//_nativeWinwdow.Maximize();
	}

	void MacOsWindow::SetVisible(bool visible)
	{
		// todo
	}
}
