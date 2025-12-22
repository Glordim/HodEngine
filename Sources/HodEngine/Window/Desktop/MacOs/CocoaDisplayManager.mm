#include "HodEngine/Window/Pch.hpp"
#include "CocoaDisplayManager.hpp"

#include <Cocoa/Cocoa.h>

namespace hod::window
{
    void CocoaDisplayManager::CocoaInit()
    {
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp finishLaunching];
    }

	bool CocoaDisplayManager::Run()
	{
        @autoreleasepool
        {
            while (true)
            {
                NSEvent* event;
                while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:[NSDate distantFuture]
                                                    inMode:NSDefaultRunLoopMode
                                                    dequeue:YES]))
                {
                    [NSApp sendEvent:event];
                    [NSApp updateWindows];
                }
            }
        }

        return true;
	}
}
