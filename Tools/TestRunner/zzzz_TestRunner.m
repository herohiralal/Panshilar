#define PNSLR_IMPLEMENTATION
#include "../../Source/Dependencies/PNSLR_Intrinsics/Platforms.h"
#include "../../Source/Dependencies/PNSLR_Intrinsics/Compiler.h"
#include "../../Source/Dependencies/PNSLR_Intrinsics/Warnings.h"
PNSLR_SUPPRESS_WARN
#if PNSLR_UNIX
    #define _POSIX_C_SOURCE 200809L
    #define _XOPEN_SOURCE 700
#endif
#include <stdio.h>
PNSLR_UNSUPPRESS_WARN
#include "zzzz_TestRunner.h"

#ifdef __cplusplus
extern "C" {
#endif

    void TestRunnerMain(PNSLR_ArraySlice(utf8str) args);

#ifdef __cplusplus
}
#endif

#undef nil

#if PNSLR_IOS

#import <UIKit/UIKit.h>

static PNSLR_ArraySlice(utf8str) G_PNSLR_Internal_IOSArgs;

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Create a window if needed (iOS 13+ doesn't auto-create)
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = [[UIViewController alloc] init];
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    TestRunnerMain(G_PNSLR_Internal_IOSArgs);
    return YES;
}

@end

int main(int argc, char * argv[]) {
    PNSLR_ArraySlice(utf8str) args = PNSLR_MakeSlice(utf8str, argc, false, PNSLR_GetAllocator_DefaultHeap(), PNSLR_GET_LOC(), nullptr);
    for (i32 i = 0; i < argc; ++i) { args.data[i] = PNSLR_StringFromCString(argv[i]); }
    
    G_PNSLR_Internal_IOSArgs = args;

    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}

#endif
