#include "MacOsWindow.hpp"
#include "HodEngine/Window/Surface.hpp"

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

@interface MyWindowDelegate : NSObject <NSWindowDelegate>
{
	hod::window::MacOsWindow* _window;
}

- (instancetype)initWithWindow:(hod::window::MacOsWindow*)initWindow;
@end

@implementation MyWindowDelegate

- (instancetype)initWithWindow:(hod::window::MacOsWindow*)initWindow
{
	self = [super init];
	if (self != nil)
	{
		_window = initWindow;
	}	

	return self;
}

- (void)windowDidResize:(NSNotification *)notification
{
	_window->ResizeContext();
}
@end

namespace hod::window
{
	/// @brief 
	MacOsWindow::MacOsWindow(bool hidden)
		: DesktopWindow()
	{
		_delegate = [[MyWindowDelegate alloc] initWithWindow:this];

		NSRect frame = NSMakeRect(0, 0, _width, _height);
        _window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

        [_window setDelegate:_delegate];
		[_window setTitle:@"Window"];

        CustomView *customView = [[CustomView alloc] initWithFrame:frame];
		_view = customView;
		_view.wantsLayer = YES;

        //[_window.contentView addSubview:customView];
		[_window setContentView:customView];

		SetVisible(!hidden);
	}

	/// @brief 
	MacOsWindow::~MacOsWindow()
	{
		
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

        dispatch_async(dispatch_get_main_queue(), ^{
            NSSize newSize = NSMakeSize(_width, _height);
            [_window setContentSize:newSize];
        });
	}

	/// @brief 
	void MacOsWindow::CenterToScreen()
	{
        dispatch_async(dispatch_get_main_queue(), ^{
            [_window center];
        });
	}

	/// @brief 
	void MacOsWindow::Maximize()
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			NSScreen *screen = [NSScreen mainScreen];
			NSRect screenRect = [screen frame];
			[_window setFrame:screenRect display:YES animate:YES];
		});
	}

	void MacOsWindow::SetVisible(bool visible)
	{
		if (visible == false)
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				[_window orderOut:nil];
			});
		}
		else
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				[_window makeKeyAndOrderFront:nil];
			});
		}
	}

	float MacOsWindow::GetScaleFactor() const
	{
		CGFloat scale = [_window.screen backingScaleFactor];
		return scale;
	}

	void MacOsWindow::ResizeContext()
	{
		NSRect contentRect = [_window contentRectForFrameRect:[_window frame]];
    	NSSize contentSize = contentRect.size;

		Surface* surface = GetSurface();
        surface->Resize(contentSize.width, contentSize.height);
	}
}
