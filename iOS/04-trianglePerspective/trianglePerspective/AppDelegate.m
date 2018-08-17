#import "AppDelegate.h"
#import "ViewController.h"
#import "GLESView.h"

@interface AppDelegate ()
{
@private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    GLESView *mainView;
}

@end

@implementation AppDelegate

@synthesize window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    CGRect rect = [[UIScreen mainScreen] bounds];

    mainWindow = [[UIWindow alloc] initWithFrame:rect];
    mainViewController = [[ViewController alloc] init];
    mainView = [[GLESView alloc] initWithFrame:rect];

    [mainViewController setView:mainView];
    [mainWindow addSubview:[mainViewController view]];
    [mainWindow setRootViewController:mainViewController];
    [mainWindow makeKeyAndVisible];

    [mainView startRendering];
    [mainView release];

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [mainView stopRendering];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [mainView startRendering];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [mainView stopRendering];
}

-(void)dealloc
{
    [mainView release];
    [mainViewController release];
    [mainWindow release];
    [window release];
    
    mainView = nil;
    mainViewController = nil;
    mainWindow = nil;
    window = nil;
    
    [super dealloc];
}

@end
