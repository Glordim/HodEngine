#include "CocoaDisplayManager.hpp"
#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/Window.hpp"

#include <Cocoa/Cocoa.h>

namespace hod::inline window {
void CocoaDisplayManager::CocoaInit() {
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  [NSApp finishLaunching];
}

void CocoaDisplayManager::Update()
{
  @autoreleasepool {
      NSEvent *event;
      while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                         untilDate:[NSDate distantPast]
                                            inMode:NSDefaultRunLoopMode
                                           dequeue:YES])) {
        [NSApp sendEvent:event];
        [NSApp updateWindows];
      }
  }

  for (Window* window : _windows)
  {
    window->Update();
  }
}

// AppKit screen space is bottom-left origin, Y growing up, in points; the
// engine's desktop space is top-left origin, Y growing down (see
// MacOsWindow::SetPosition), so flip using the primary screen's height.
// Points are kept (not pixels) to match window positions and mouse events.
Vector<DesktopDisplayManager::Monitor> CocoaDisplayManager::GetMonitors() const {
  Vector<Monitor> monitors;
  NSArray<NSScreen *> *screens = [NSScreen screens];
  if ([screens count] == 0) {
    return monitors;
  }

  // screens[0] is the one holding the menu bar, i.e. the primary monitor.
  CGFloat primaryHeight = [screens[0] frame].size.height;
  for (NSUInteger i = 0; i < [screens count]; ++i) {
    NSRect frame = [screens[i] frame];
    NSRect visibleFrame = [screens[i] visibleFrame]; // Excludes the Dock and the menu bar

    Monitor monitor;
    monitor._position = Vector2(static_cast<float>(frame.origin.x),
                                static_cast<float>(primaryHeight - frame.origin.y - frame.size.height));
    monitor._size = Vector2(static_cast<float>(frame.size.width),
                            static_cast<float>(frame.size.height));
    monitor._workPosition = Vector2(static_cast<float>(visibleFrame.origin.x),
                                    static_cast<float>(primaryHeight - visibleFrame.origin.y - visibleFrame.size.height));
    monitor._workSize = Vector2(static_cast<float>(visibleFrame.size.width),
                                static_cast<float>(visibleFrame.size.height));
    monitor._primary = (i == 0);
    monitors.PushBack(monitor);
  }
  return monitors;
}
}
