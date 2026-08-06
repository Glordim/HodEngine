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
}
