
#import "MacWindowEffects.h"
#import <Cocoa/Cocoa.h>

@interface WindowLoader : NSObject
+ (void)applyVibrancyToWindow:(void*)windowId;
@end

@implementation WindowLoader

+ (void)applyVibrancyToWindow:(void*)windowId {
    NSView* view = (__bridge NSView*)windowId;
    NSWindow* window = [view window];

    if (!window)
        return;

    // Transparent background
    [window setBackgroundColor:[NSColor clearColor]];
    [window setOpaque:NO];
    [window setHasShadow:YES];

    // Create visual effect view
    NSVisualEffectView* visualEffectView = [[NSVisualEffectView alloc] initWithFrame:[[window contentView] bounds]];
    [visualEffectView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

    // Material
    [visualEffectView setMaterial:NSVisualEffectMaterialSidebar];
    [visualEffectView setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
    [visualEffectView setState:NSVisualEffectStateActive];

    // Add to window
    // We add it as the first subview to be behind everything else
    [[window contentView] addSubview:visualEffectView positioned:NSWindowBelow relativeTo:nil];

    // Setup titlebar
    window.titlebarAppearsTransparent = YES;
    window.styleMask |= NSWindowStyleMaskFullSizeContentView;
}

@end

void MacWindowEffects::applyVibrancy(WId windowId) {
    [WindowLoader applyVibrancyToWindow:(void*)windowId];
}
