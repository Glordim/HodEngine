#include "MacOsWindow.hpp"

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
		NSRect frame = NSMakeRect(0, 0, 400, 300);
        _window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

        [_window setTitle:@"Window"];

        CustomView *customView = [[CustomView alloc] initWithFrame:frame];
		_view = customView;

        [_window.contentView addSubview:customView];

        [_window makeKeyAndOrderFront:nil];
	}

	/// @brief 
	MacOsWindow::~MacOsWindow()
	{
		
	}

	/// @brief 
	void MacOsWindow::Update()
	{
		// Traiter les événements
		NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
												untilDate:[NSDate distantPast]
												inMode:NSDefaultRunLoopMode
												dequeue:YES];
		if (event != nil) {
			[NSApp sendEvent:event];
		}
	}

	/// @brief 
	/// @param width 
	/// @param height 
	void MacOsWindow::SetSize(uint16_t width, uint16_t height)
	{
		_width = width;
		_height = height;

		NSRect newFrame = NSMakeRect(0, 0, _width, _height);
        [_window setFrame:newFrame display:YES];
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
}
