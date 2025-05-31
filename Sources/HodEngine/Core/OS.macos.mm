#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/OS.hpp"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

namespace hod
{
	void OS::OpenFileWithDefaultApp(const char* filePath)
	{
		@autoreleasepool
		{
			NSString* nsPath = [NSString stringWithUTF8String:filePath];
			NSURL* fileURL = [NSURL fileURLWithPath:nsPath];
			[[NSWorkspace sharedWorkspace] openURL:fileURL];
		}
	}
}
