#include "HodEngine/Core/OS.hpp"
#include "HodEngine/Core/Pch.hpp"


#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>


namespace hod::inline core {
bool OS::OpenFileWithDefaultApp(const char *filePath) {
  @autoreleasepool {
    NSString *nsPath = [NSString stringWithUTF8String:filePath];
    NSURL *fileURL = [NSURL fileURLWithPath:nsPath];
    [[NSWorkspace sharedWorkspace] openURL:fileURL];
  }
  return true;
}
} // namespace hod::inline core
