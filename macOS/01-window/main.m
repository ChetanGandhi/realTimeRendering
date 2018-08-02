#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];
    [NSApp run];
    
    [pool release];
    
    return 0;
}
