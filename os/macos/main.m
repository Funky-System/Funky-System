#import <Cocoa/Cocoa.h>

#import "../../funky_system.h"

extern const int INITIAL_SCALE;
NSMenuItem *menuItemScale1, *menuItemScale2, *menuItemScale3, *menuItemScale4;

void updateScaleMenuItems() {
    int scale = INITIAL_SCALE;
    if (get_display()->scale != 0) scale = get_display()->scale;

    [menuItemScale1 setState:NSOffState];
    [menuItemScale2 setState:NSOffState];
    [menuItemScale3 setState:NSOffState];
    [menuItemScale4 setState:NSOffState];
    if (scale == 1) [menuItemScale1 setState:NSOnState];
    if (scale == 2) [menuItemScale2 setState:NSOnState];
    if (scale == 3) [menuItemScale3 setState:NSOnState];
    if (scale == 4) [menuItemScale4 setState:NSOnState];
}

@interface Actions : NSObject

+ (void)setScale1:(id)sender;
+ (void)setScale2:(id)sender;
+ (void)setScale3:(id)sender;
+ (void)setScale4:(id)sender;

@end

@implementation Actions

+ (void)setScale1:(id)sender {
    display_set_scale(get_display(), 1);
    updateScaleMenuItems();
}

+ (void)setScale2:(id)sender {
    display_set_scale(get_display(), 2);
    updateScaleMenuItems();
}

+ (void)setScale3:(id)sender {
    display_set_scale(get_display(), 3);
    updateScaleMenuItems();
}

+ (void)setScale4:(id)sender {
    display_set_scale(get_display(), 4);
    updateScaleMenuItems();
}

@end

const char* getBundleResourcesDir() {
    NSBundle *bundle = [NSBundle mainBundle];
    return [[bundle resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];
}

void SetProcessName(CFStringRef process_name);

void initNativeWindow() {
    //[[NSBundle mainBundle] loadNibNamed:@"MainMenu" owner:nil topLevelObjects:nil];

    if (NSApp) {
        NSBundle *bundle = [NSBundle mainBundle];
        //[NSApp setApplicationIconImage:[[[NSImage new] autorelease] initWithContentsOfFile:@"Icon.png"]];
        [NSApp setApplicationIconImage:[NSImage imageNamed:@"Icon.png"]];

        NSString* processName = [[NSProcessInfo processInfo] processName];
        NSString* appName = @"Funky System";

        //NSMenu* menubar = [[NSMenu new] autorelease];
        NSMenu* menubar = NSApp.mainMenu;

        //[NSApp setMainMenu:menubar];
        [menubar removeAllItems];

        // App menu
        NSMenuItem* appMenuItem = [[NSMenuItem new] autorelease];
        NSMenu* appMenu = [[NSMenu new] autorelease];
        NSString* quitTitle = [@"Quit " stringByAppendingString:appName];
        NSMenuItem* quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
                                                               action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
        [appMenu addItemWithTitle:@"About..." action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
        [appMenu addItem: [NSMenuItem separatorItem]];
        [appMenu addItem:quitMenuItem];

        [appMenuItem setSubmenu:appMenu];
        [menubar addItem:appMenuItem];

        NSMenuItem* displayMenuItem = [[NSMenuItem new] autorelease];
        NSMenu* displayMenu = [[NSMenu new] autorelease];
        [displayMenu setTitle:@"Display"];
        menuItemScale1 = [displayMenu addItemWithTitle:@"Scale 1x" action:@selector(setScale1:) keyEquivalent:@"1"];
        menuItemScale2 = [displayMenu addItemWithTitle:@"Scale 2x" action:@selector(setScale2:) keyEquivalent:@"2"];
        menuItemScale3 = [displayMenu addItemWithTitle:@"Scale 3x" action:@selector(setScale3:) keyEquivalent:@"3"];
        menuItemScale4 = [displayMenu addItemWithTitle:@"Scale 4x" action:@selector(setScale4:) keyEquivalent:@"4"];
        [menuItemScale1 setTarget:[Actions class]];
        [menuItemScale2 setTarget:[Actions class]];
        [menuItemScale3 setTarget:[Actions class]];
        [menuItemScale4 setTarget:[Actions class]];
        updateScaleMenuItems();

        [displayMenuItem setSubmenu:displayMenu];
        [menubar addItem:displayMenuItem];
    }
}

int platform_handle_event(SDL_Event *event, CPU_State* state) {
    return 0;
}
