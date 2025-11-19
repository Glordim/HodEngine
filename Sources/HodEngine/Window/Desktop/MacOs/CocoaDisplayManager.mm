#include "HodEngine/Window/Pch.hpp"
#include "CocoaDisplayManager.hpp"

#include <Cocoa/Cocoa.h>

namespace hod::window
{
	void CocoaDisplayManager::Update()
	{
        @autoreleasepool
        {
            NSEvent *event;
            while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                               untilDate:nil
                                                  inMode:NSDefaultRunLoopMode
                                                 dequeue:YES]))
            {
                [NSApp sendEvent:event];
                [NSApp updateWindows];
            }
        }
	}
}
