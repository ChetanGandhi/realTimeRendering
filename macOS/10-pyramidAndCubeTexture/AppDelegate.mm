#import "AppDelegate.h"
#import "GLView.h"

@implementation AppDelegate
{
@private
    NSWindow *window;
    GLView *view;
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSRect windowRect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    view = [[GLView alloc] initWithFrame:windowRect];

    window = [[NSWindow alloc] initWithContentRect:windowRect
                                         styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window setTitle:@"Pyramid and Cube Texture"];
    [window center];
    [window setContentView: view];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification *)notification
{
    NSLog(@"applicationWillTerminate");
}

-(void)windowWillClose:(NSNotification *)notification
{
    [NSApp terminate:self];
}

-(void)dealloc
{
    [view release];
    [window release];
    [super dealloc];
}

@end
