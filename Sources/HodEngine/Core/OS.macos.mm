#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Pch.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

namespace hod::inline core
{
	bool OS::OpenFileWithDefaultApp(const char *filePath)
	{
		@autoreleasepool {
			NSString *nsPath = [NSString stringWithUTF8String:filePath];
			NSURL *fileURL = [NSURL fileURLWithPath:nsPath];
			[[NSWorkspace sharedWorkspace] openURL:fileURL];
		}
		return true;
	}

	bool OS::ReadClipboard(String& value)
	{
		@autoreleasepool {
			NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
			NSString* nsString = [pasteboard stringForType:NSPasteboardTypeString];
			if (nsString == nil)
				return false;

			const char* utf8 = [nsString UTF8String];
			if (utf8 == nullptr)
				return false;

			value = utf8;
		}
		return true;
	}

	bool OS::WriteClipboard(const std::string_view& value)
	{
		@autoreleasepool {
			NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
			[pasteboard clearContents];
			NSString* nsString = [[NSString alloc] initWithBytes:value.data() length:value.size() encoding:NSUTF8StringEncoding];
			if (nsString == nil)
				return false;

			return [pasteboard setString:nsString forType:NSPasteboardTypeString] == YES;
		}
	}
}
