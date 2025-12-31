#include "MacVisualEffectItem.h"

#ifdef Q_OS_MACOS

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#include <QQuickWindow>

MacVisualEffectItem::MacVisualEffectItem(QQuickItem* parent) : QQuickItem(parent) {
    setFlag(ItemHasContents, false);
}

MacVisualEffectItem::~MacVisualEffectItem() {
    if (m_effectView) {
        NSVisualEffectView* view = (__bridge_transfer NSVisualEffectView*)m_effectView;
        [view removeFromSuperview];
        m_effectView = nullptr;
    }
}

void MacVisualEffectItem::setMaterial(Material material) {
    if (m_material == material)
        return;
    m_material = material;
    updateEffect();
    emit materialChanged();
}

void MacVisualEffectItem::setBlendingMode(BlendingMode mode) {
    if (m_blendingMode == mode)
        return;
    m_blendingMode = mode;
    updateEffect();
    emit blendingModeChanged();
}

void MacVisualEffectItem::componentComplete() {
    QQuickItem::componentComplete();
    updateEffect();
}

void MacVisualEffectItem::itemChange(ItemChange change, const ItemChangeData& value) {
    QQuickItem::itemChange(change, value);
    if (change == ItemSceneChange && value.window) {
        updateEffect();
    }
}

void MacVisualEffectItem::updateEffect() {
    if (!window())
        return;

    // Get the native NSWindow
    NSView* nativeView = (__bridge NSView*)reinterpret_cast<void*>(window()->winId());
    if (!nativeView)
        return;

    NSWindow* nsWindow = [nativeView window];
    if (!nsWindow)
        return;

    // Create or update the visual effect view
    NSVisualEffectView* effectView;
    if (m_effectView) {
        effectView = (__bridge NSVisualEffectView*)m_effectView;
    } else {
        effectView = [[NSVisualEffectView alloc] initWithFrame:[[nsWindow contentView] bounds]];
        [effectView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [[nsWindow contentView] addSubview:effectView positioned:NSWindowBelow relativeTo:nil];
        m_effectView = (__bridge_retained void*)effectView;
    }

    // Set material
    NSVisualEffectMaterial nsMaterial;
    switch (m_material) {
        case Titlebar:
            nsMaterial = NSVisualEffectMaterialTitlebar;
            break;
        case Selection:
            nsMaterial = NSVisualEffectMaterialSelection;
            break;
        case Menu:
            nsMaterial = NSVisualEffectMaterialMenu;
            break;
        case Popover:
            nsMaterial = NSVisualEffectMaterialPopover;
            break;
        case Sidebar:
            nsMaterial = NSVisualEffectMaterialSidebar;
            break;
        case HeaderView:
            nsMaterial = NSVisualEffectMaterialHeaderView;
            break;
        case Sheet:
            nsMaterial = NSVisualEffectMaterialSheet;
            break;
        case WindowBackground:
            nsMaterial = NSVisualEffectMaterialWindowBackground;
            break;
        case HUDWindow:
            nsMaterial = NSVisualEffectMaterialHUDWindow;
            break;
        case FullScreenUI:
            nsMaterial = NSVisualEffectMaterialFullScreenUI;
            break;
        case ToolTip:
            nsMaterial = NSVisualEffectMaterialToolTip;
            break;
        case ContentBackground:
            nsMaterial = NSVisualEffectMaterialContentBackground;
            break;
        case UnderWindowBackground:
            nsMaterial = NSVisualEffectMaterialUnderWindowBackground;
            break;
        case UnderPageBackground:
            nsMaterial = NSVisualEffectMaterialUnderPageBackground;
            break;
        default:
            nsMaterial = NSVisualEffectMaterialSidebar;
            break;
    }
    [effectView setMaterial:nsMaterial];

    // Set blending mode
    NSVisualEffectBlendingMode nsBlendingMode;
    switch (m_blendingMode) {
        case BehindWindow:
            nsBlendingMode = NSVisualEffectBlendingModeBehindWindow;
            break;
        case WithinWindow:
            nsBlendingMode = NSVisualEffectBlendingModeWithinWindow;
            break;
        default:
            nsBlendingMode = NSVisualEffectBlendingModeBehindWindow;
            break;
    }
    [effectView setBlendingMode:nsBlendingMode];

    [effectView setState:NSVisualEffectStateActive];

    // Configure window for transparency
    [nsWindow setBackgroundColor:[NSColor clearColor]];
    [nsWindow setOpaque:NO];
    nsWindow.titlebarAppearsTransparent = YES;
    nsWindow.styleMask |= NSWindowStyleMaskFullSizeContentView;
}

#endif  // Q_OS_MACOS
