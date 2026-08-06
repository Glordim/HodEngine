#include "MacOsWindow.hpp"
#include "HodEngine/Window/Pch.hpp"

#include <Cocoa/Cocoa.h>

static const NSRange kEmptyRange = {NSNotFound, 0};

@interface CustomView : NSView <NSTextInputClient> {
  hod::MacOsWindow *window;
  NSMutableAttributedString *markedText;
  NSEventModifierFlags previousModifierFlags;
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
    markedText = [[NSMutableAttributedString alloc] init];
    previousModifierFlags = 0;
  }
  return self;
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

- (void)drawRect:(NSRect)dirtyRect {
  [[NSColor whiteColor] setFill];
  NSRectFill(dirtyRect);
  [super drawRect:dirtyRect];
}

// Top-left origin, Y growing down: matches the coordinate space ImGui (and
// the rest of the engine's Window lib) expects, so no manual flip is needed
// when reporting mouse positions.
- (BOOL)isFlipped {
  return YES;
}

// NSWindow.acceptsMouseMovedEvents alone is not reliably enough to get
// mouseMoved: called; a tracking area with NSTrackingMouseMoved is the
// robust way to receive hover events regardless of that window flag.
- (void)updateTrackingAreas {
  [super updateTrackingAreas];
  for (NSTrackingArea *area in self.trackingAreas) {
    [self removeTrackingArea:area];
  }
  NSTrackingAreaOptions options = NSTrackingMouseMoved |
                                   NSTrackingActiveAlways |
                                   NSTrackingInVisibleRect;
  NSTrackingArea *trackingArea =
      [[NSTrackingArea alloc] initWithRect:self.bounds
                                    options:options
                                      owner:self
                                   userInfo:nil];
  [self addTrackingArea:trackingArea];
}

- (void)keyDown:(NSEvent *)event {
  uint16_t keyCode = [event keyCode];
  hod::MacOsWindowEventCaller::EmitKeyPressed(window, keyCode);

  // AppKit never sends keyUp: for a non-modifier key that was pressed while
  // Command is held (well-known AppKit quirk: Cmd+key is treated as a
  // one-shot "key equivalent", not a held key). Left stuck "down", it would
  // keep re-triggering repeat-eligible actions (e.g. ImGui's Cmd+X cut)
  // every frame until Command is eventually released. Since no real keyUp:
  // is coming, synthesize it immediately instead.
  if (([event modifierFlags] & NSEventModifierFlagCommand) != 0) {
    hod::MacOsWindowEventCaller::EmitKeyReleased(window, keyCode);
  }

  // Routes the event through the input method: for a plain keystroke this
  // calls insertText:replacementRange: synchronously below, but for dead
  // keys / IME composition (accents, Chinese/Japanese/Korean, ...) it lets
  // the system's composition session run, only calling insertText: once
  // the user has actually committed the composed text.
  [self interpretKeyEvents:@[ event ]];
}

- (void)keyUp:(NSEvent *)event {
  hod::MacOsWindowEventCaller::EmitKeyReleased(window, [event keyCode]);
}

// Modifier keys (Shift/Control/Option/Command) never generate keyDown:/
// keyUp: on macOS, only flagsChanged:; without this, Key::Ctrl/Shift/Alt/
// Super are never reported, so Cmd/Ctrl-based shortcuts never see their
// modifier as held.
- (void)flagsChanged:(NSEvent *)event {
  NSEventModifierFlags newFlags = [event modifierFlags];
  NSEventModifierFlags changed = newFlags ^ previousModifierFlags;
  previousModifierFlags = newFlags;

  NSEventModifierFlags relevant = changed & (NSEventModifierFlagShift |
                                              NSEventModifierFlagControl |
                                              NSEventModifierFlagOption |
                                              NSEventModifierFlagCommand |
                                              NSEventModifierFlagCapsLock);
  if (relevant == 0) {
    return;
  }

  BOOL pressed = (newFlags & relevant) != 0;

  if (pressed) {
    hod::MacOsWindowEventCaller::EmitKeyPressed(window, [event keyCode]);
  } else {
    hod::MacOsWindowEventCaller::EmitKeyReleased(window, [event keyCode]);
  }
}

- (void)doCommandBySelector:(SEL)selector {
  // Swallow editing commands (moveLeft:, deleteBackward:, insertNewline:,
  // ...) that interpretKeyEvents: may synthesize; scan codes already cover
  // them via EmitKeyPressed, and leaving this unhandled would make AppKit
  // beep.
}

#pragma mark - NSTextInputClient

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {
  NSString *characters = [string isKindOfClass:[NSAttributedString class]]
                              ? [(NSAttributedString *)string string]
                              : (NSString *)string;

  NSUInteger length = [characters length];
  for (NSUInteger i = 0; i < length; ++i) {
    unichar c = [characters characterAtIndex:i];
    if (c >= 0xF700 && c <= 0xF8FF) {
      continue;
    }
    hod::MacOsWindowEventCaller::EmitChar(window, static_cast<char>(c));
  }
}

- (BOOL)hasMarkedText {
  return [markedText length] > 0;
}

- (NSRange)markedRange {
  if ([markedText length] > 0) {
    return NSMakeRange(0, [markedText length]);
  }
  return kEmptyRange;
}

- (NSRange)selectedRange {
  return kEmptyRange;
}

- (void)setMarkedText:(id)string
        selectedRange:(NSRange)selectedRange
     replacementRange:(NSRange)replacementRange {
  if ([string isKindOfClass:[NSAttributedString class]]) {
    markedText = [[NSMutableAttributedString alloc] initWithAttributedString:string];
  } else {
    markedText = [[NSMutableAttributedString alloc] initWithString:string];
  }
}

- (void)unmarkText {
  [[markedText mutableString] setString:@""];
}

- (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText {
  return @[];
}

- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)range
                                                  actualRange:(NSRangePointer)actualRange {
  return nil;
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point {
  return 0;
}

- (NSRect)firstRectForCharacterRange:(NSRange)range
                          actualRange:(NSRangePointer)actualRange {
  // No inline composition UI: park the candidate window at the view's
  // origin rather than tracking the caret.
  NSRect frame = self.frame;
  return [self.window convertRectToScreen:frame];
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

// AppKit only sends mouseMoved: while no button is down; while dragging it
// sends these instead, so they need the same handling to keep reporting
// position during a drag.
- (void)mouseDragged:(NSEvent *)event {
  [self mouseMoved:event];
}

- (void)rightMouseDragged:(NSEvent *)event {
  [self mouseMoved:event];
}

- (void)otherMouseDragged:(NSEvent *)event {
  [self mouseMoved:event];
}

- (void)scrollWheel:(NSEvent *)event {
  double deltaX = [event scrollingDeltaX];
  double deltaY = [event scrollingDeltaY];
  // Trackpads report precise pixel deltas that are an order of magnitude
  // larger than the "line" deltas from a physical mouse wheel; scale them
  // down so both devices produce comparable scroll amounts.
  if ([event hasPreciseScrollingDeltas]) {
    deltaX *= 0.01;
    deltaY *= 0.01;
  }

  if (deltaY != 0.0) {
    hod::MacOsWindowEventCaller::EmitMouseScroll(window, deltaY);
  }
  if (deltaX != 0.0) {
    hod::MacOsWindowEventCaller::EmitMouseHorizontalScroll(window, deltaX);
  }
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

- (void)windowDidMove:(NSNotification *)notification {
  _window->UpdatePositionFromNative();
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
  [_window setTitle:@"Hod Engine"];
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
void MacOsWindow::RunOnMainThread(std::function<void()> codeToRun) {
  if ([NSThread isMainThread]) {
    codeToRun();
  } else {
    dispatch_async(dispatch_get_main_queue(), ^{
      codeToRun();
    });
  }
}

/// @brief
/// @param width
/// @param height
void MacOsWindow::SetSize(uint16_t width, uint16_t height) {
  RunOnMainThread([this, width, height]() {
    NSSize newSize = NSMakeSize(width, height);
    [_window setContentSize:newSize];
  });
}

/// @brief
void MacOsWindow::CenterToScreen() {
  RunOnMainThread([this]() {
    [_window center];
  });
}

/// @brief
void MacOsWindow::Maximize() {
  RunOnMainThread([this]() {
    NSScreen *screen = [NSScreen mainScreen];
    NSRect screenRect = [screen frame];
    [_window setFrame:screenRect display:YES animate:YES];
  });
}

void MacOsWindow::SetVisible(bool visible) {
  if (visible == false) {
    RunOnMainThread([this]() {
      [_window orderOut:nil];
    });
  } else {
    RunOnMainThread([this]() {
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

/// @brief
/// @param title
void MacOsWindow::SetTitle(const char* title) {
  NSString *nsTitle = [NSString stringWithUTF8String:title];
  RunOnMainThread([this, nsTitle]() {
    [_window setTitle:nsTitle];
  });
}

/// @brief
/// @param decoration
void MacOsWindow::SetDecoration(bool decoration) {
  RunOnMainThread([this, decoration]() {
    if (decoration == false) {
      [_window setStyleMask:NSWindowStyleMaskBorderless];
    } else {
      [_window setStyleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                             NSWindowStyleMaskResizable];
    }
  });
}

/// @brief
/// @param position
// position is the content view's top-left, top-left origin, Y growing down
// (matches the rest of the engine's Window lib, see MacOsWindow.mm:41, and
// the content-view-local coordinates mouse events are reported in), while
// AppKit's screen space is bottom-left origin, Y growing up: flip using the
// primary screen's height (NSScreen.screens[0] is always anchored at (0, 0)).
// Using the content rect rather than the full frame keeps this consistent
// with ImGuiManager's mouse-pos-event math (window-local mouse pos + this
// position), which would otherwise be off by the title bar height on
// decorated windows.
void MacOsWindow::SetPosition(const Vector2& position) {
  RunOnMainThread([this, position]() {
    NSRect contentRect = [_window contentRectForFrameRect:[_window frame]];
    CGFloat screenHeight = [[[NSScreen screens] firstObject] frame].size.height;
    contentRect.origin = NSMakePoint(position.GetX(), screenHeight - position.GetY() - contentRect.size.height);
    NSRect newFrame = [_window frameRectForContentRect:contentRect];
    [_window setFrameOrigin:newFrame.origin];
  });
}

/// @brief
void MacOsWindow::UpdatePositionFromNative() {
  NSRect contentRect = [_window contentRectForFrameRect:[_window frame]];
  CGFloat screenHeight = [[[NSScreen screens] firstObject] frame].size.height;
  _position.SetX(static_cast<float>(contentRect.origin.x));
  _position.SetY(static_cast<float>(screenHeight - contentRect.origin.y - contentRect.size.height));
}
}
