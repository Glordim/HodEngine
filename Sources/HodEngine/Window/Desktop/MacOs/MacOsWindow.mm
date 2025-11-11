#include "HodEngine/Window/Pch.hpp"
#include "MacOsWindow.hpp"
#include "HodEngine/Window/Surface.hpp"

#include <Cocoa/Cocoa.h>

@interface CustomView : NSView
{
	hod::window::MacOsWindow* window;
}

- (instancetype)initWithFrame:(NSRect)frameRect window:(hod::window::MacOsWindow*)cppWindow;
@end

@implementation CustomView

- (instancetype)initWithFrame:(NSRect)frameRect window:(hod::window::MacOsWindow*)cppWindow
{
    self = [super initWithFrame:frameRect];
    if (self)
	{
        window = cppWindow;
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
	[[NSColor whiteColor] setFill];
	NSRectFill(dirtyRect);
	[super drawRect:dirtyRect];
}

- (void)keyDown:(NSEvent*)event
{
    window->EmitKeyPressed([event keyCode]);
}

- (void)keyUp:(NSEvent*)event
{
    window->EmitKeyReleased([event keyCode]);
}

- (void)mouseDown:(NSEvent*)event
{
    window->EmitMouseButtonPressed(hod::window::MouseButton::Left);
}

- (void)mouseUp:(NSEvent*)event
{
    window->EmitMouseButtonReleased(hod::window::MouseButton::Left);
}

- (void)rightMouseDown:(NSEvent*)event
{
    window->EmitMouseButtonPressed(hod::window::MouseButton::Right);
}

- (void)rightMouseUp:(NSEvent*)event
{
    window->EmitMouseButtonReleased(hod::window::MouseButton::Right);
}

- (void)otherMouseDown:(NSEvent*)event
{
    window->EmitMouseButtonPressed(hod::window::MouseButton::Middle);
}

- (void)otherMouseUp:(NSEvent*)event
{
    window->EmitMouseButtonReleased(hod::window::MouseButton::Middle);
}

- (void)mouseMoved:(NSEvent*)event
{
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    window->EmitMouseMoved(location.x, location.y);
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

- (BOOL)windowShouldClose:(NSWindow *)sender
{
    return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
	_window->Close();
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
        [NSApplication sharedApplication];
        
		_delegate = [[MyWindowDelegate alloc] initWithWindow:this];

		NSRect frame = NSMakeRect(0, 0, _width, _height);
        _window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

        [_window setDelegate:_delegate];
		[_window setTitle:@"Window"];
        [_window makeKeyAndOrderFront:(nil)];

        CustomView *customView = [[CustomView alloc] initWithFrame:frame window:this];
		_view = customView;
		_view.wantsLayer = YES;

        //[_window.contentView addSubview:customView];
		[_window setContentView:customView];

		SetVisible(!hidden);
        
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
	}

	/// @brief 
	MacOsWindow::~MacOsWindow()
	{
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
