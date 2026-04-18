#include "MacOsWindow.hpp"
#include "HodEngine/Window/Pch.hpp"

#include <Cocoa/Cocoa.h>

@interface CustomView : NSView {
  hod::MacOsWindow *window;
}

- (instancetype)initWithFrame:(NSRect)frameRect
                       window:(hod::MacOsWindow *)cppWindow;
@end

@implementation CustomView

- (instancetype)initWithFrame:(NSRect)frameRect
                       window:(hod::MacOsWindow *)cppWindow {
  self = [super initWithFrame:frameRect];
  if (self) {
    window = cppWindow;
  }
  return self;
}

- (void)drawRect:(NSRect)dirtyRect {
  [[NSColor whiteColor] setFill];
  NSRectFill(dirtyRect);
  [super drawRect:dirtyRect];
}

- (void)keyDown:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitKeyPressed(
      window, hod::MacOSKeyCodeToScanCode([event keyCode]));
}

- (void)keyUp:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitKeyReleased(
      window, hod::MacOSKeyCodeToScanCode([event keyCode]));
}

- (void)mouseDown:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitMouseButtonPressed(window,
                                                      hod::MouseButton::Left);
}

- (void)mouseUp:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitMouseButtonReleased(window,
                                                       hod::MouseButton::Left);
}

- (void)rightMouseDown:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitMouseButtonPressed(window,
                                                      hod::MouseButton::Right);
}

- (void)rightMouseUp:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitMouseButtonReleased(window,
                                                       hod::MouseButton::Right);
}

- (void)otherMouseDown:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitMouseButtonPressed(window,
                                                      hod::MouseButton::Middle);
}

- (void)otherMouseUp:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitMouseButtonReleased(
      window, hod::MouseButton::Middle);
}

- (void)mouseMoved:(NSEvent *)event {
  NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
  hod::MacOsWindowEventCaller::EmitMouseMoved(window, location.x, location.y);
}

@end

@interface MyWindowDelegate : NSObject <NSWindowDelegate> {
  hod::MacOsWindow *_window;
}

- (instancetype)initWithWindow:(hod::MacOsWindow *)initWindow;
@end

@implementation MyWindowDelegate

- (instancetype)initWithWindow:(hod::MacOsWindow *)initWindow {
  self = [super init];
  if (self != nil) {
    _window = initWindow;
  }

  return self;
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
  return YES;
}

- (void)windowWillClose:(NSNotification *)notification {
  _window->Close();
}

- (void)windowDidResize:(NSNotification *)notification {
  _window->ResizeContext();
}
@end

namespace hod::inline window {
/// @brief
MacOsWindow::MacOsWindow(bool hidden) : DesktopWindow() {
  [NSApplication sharedApplication];

  _delegate = [[MyWindowDelegate alloc] initWithWindow:this];

  NSRect frame = NSMakeRect(0, 0, _width, _height);
  _window = [[NSWindow alloc]
      initWithContentRect:frame
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                          NSWindowStyleMaskResizable
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
MacOsWindow::~MacOsWindow() {}

/// @brief
/// @param width
/// @param height
void MacOsWindow::SetSize(uint16_t width, uint16_t height) {
  dispatch_async(dispatch_get_main_queue(), ^{
    NSSize newSize = NSMakeSize(width, height);
    [_window setContentSize:newSize];
  });
}

/// @brief
void MacOsWindow::CenterToScreen() {
  dispatch_async(dispatch_get_main_queue(), ^{
    [_window center];
  });
}

/// @brief
void MacOsWindow::Maximize() {
  dispatch_async(dispatch_get_main_queue(), ^{
    NSScreen *screen = [NSScreen mainScreen];
    NSRect screenRect = [screen frame];
    [_window setFrame:screenRect display:YES animate:YES];
  });
}

void MacOsWindow::SetVisible(bool visible) {
  if (visible == false) {
    dispatch_async(dispatch_get_main_queue(), ^{
      [_window orderOut:nil];
    });
  } else {
    dispatch_async(dispatch_get_main_queue(), ^{
      [_window makeKeyAndOrderFront:nil];
    });
  }
}

float MacOsWindow::GetScaleFactor() const {
  CGFloat scale = [_window.screen backingScaleFactor];
  return scale;
}

void MacOsWindow::ResizeContext() {
  NSRect contentRect = [_window contentRectForFrameRect:[_window frame]];
  NSSize contentSize = contentRect.size;
  hod::MacOsWindowEventCaller::EmitResize(this, contentSize.width,
                                          contentSize.height);
}
} // namespace hod::inline window
