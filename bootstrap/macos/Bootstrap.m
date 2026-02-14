#import <Cocoa/Cocoa.h>
#include <CoreFoundation/CoreFoundation.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define APP_NAME "ProjT Launcher.app"
#define INSTALL_PATH "/Applications/" APP_NAME
#define FEED_URL BOOTSTRAP_FEED_URL
#define URL_TEMPLATE BOOTSTRAP_URL_TEMPLATE

extern char **environ;

static NSWindow *g_window = nil;
static NSTextField *g_statusLabel = nil;
static NSProgressIndicator *g_progress = nil;

static void pump_events(void) {
    @autoreleasepool {
        NSEvent *event = nil;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate dateWithTimeIntervalSinceNow:0]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            [NSApp sendEvent:event];
        }
        [NSApp updateWindows];
    }
}

static void init_ui(void) {
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        NSRect frame = NSMakeRect(0, 0, 520, 280);
        g_window = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable)
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        [g_window setTitle:@"ProjT Launcher Installer"];
        [g_window center];

        NSView *content = [g_window contentView];

        NSTextField *title = [[NSTextField alloc] initWithFrame:NSMakeRect(24, 210, 472, 40)];
        [title setStringValue:@"ProjT Launcher Installer"];
        [title setBezeled:NO];
        [title setDrawsBackground:NO];
        [title setEditable:NO];
        [title setSelectable:NO];
        [title setFont:[NSFont systemFontOfSize:18 weight:NSFontWeightSemibold]];
        [content addSubview:title];

        g_statusLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(24, 165, 472, 30)];
        [g_statusLabel setStringValue:@"Preparing..."];
        [g_statusLabel setBezeled:NO];
        [g_statusLabel setDrawsBackground:NO];
        [g_statusLabel setEditable:NO];
        [g_statusLabel setSelectable:NO];
        [g_statusLabel setFont:[NSFont systemFontOfSize:13 weight:NSFontWeightRegular]];
        [content addSubview:g_statusLabel];

        g_progress = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(24, 125, 472, 12)];
        [g_progress setIndeterminate:YES];
        [g_progress setStyle:NSProgressIndicatorStyleBar];
        [g_progress startAnimation:nil];
        [content addSubview:g_progress];

        [g_window makeKeyAndOrderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];
        pump_events();
    }
}

static void update_status(const char *message) {
    if (!g_statusLabel || !message) return;
    @autoreleasepool {
        NSString *text = [NSString stringWithUTF8String:message];
        [g_statusLabel setStringValue:text ?: @""];
        pump_events();
    }
}

static void show_alert(const char *title, const char *message) {
    @autoreleasepool {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:(title ? [NSString stringWithUTF8String:title] : @"ProjT Launcher Installer")];
        [alert setInformativeText:(message ? [NSString stringWithUTF8String:message] : @"")];
        [alert addButtonWithTitle:@"OK"];
        [alert runModal];
    }
}

static int run_cmd(char *const argv[]) {
    pid_t pid = 0;
    int status = 0;
    if (posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ) != 0) {
        return -1;
    }
    if (waitpid(pid, &status, 0) < 0) {
        return -1;
    }
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
}

static char *strdup_safe(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *out = (char *)malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, s, len + 1);
    return out;
}

static char *extract_version(const char *text) {
    if (!text) return NULL;
    const char *p = text;
    while (*p && (*p < '0' || *p > '9')) p++;
    if (!*p) return NULL;
    const char *start = p;
    while (*p) {
        if ((*p >= '0' && *p <= '9') || *p == '.' || *p == '-' ) {
            p++;
            continue;
        }
        break;
    }
    size_t len = (size_t)(p - start);
    if (len == 0) return NULL;
    char *out = (char *)malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, start, len);
    out[len] = '\0';
    return out;
}

static int version_compare(const char *a, const char *b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    char *ca = strdup_safe(a);
    char *cb = strdup_safe(b);
    if (!ca || !cb) {
        free(ca);
        free(cb);
        return 0;
    }
    const char *delim = "._-";
    char *sa = strtok(ca, delim);
    char *sb = strtok(cb, delim);
    while (sa || sb) {
        long va = sa ? strtol(sa, NULL, 10) : 0;
        long vb = sb ? strtol(sb, NULL, 10) : 0;
        if (va < vb) { free(ca); free(cb); return -1; }
        if (va > vb) { free(ca); free(cb); return 1; }
        sa = sa ? strtok(NULL, delim) : NULL;
        sb = sb ? strtok(NULL, delim) : NULL;
    }
    free(ca);
    free(cb);
    return 0;
}

static char *get_installed_version(void) {
    CFURLRef appURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                              (const UInt8 *)INSTALL_PATH,
                                                              strlen(INSTALL_PATH),
                                                              true);
    if (!appURL) return NULL;
    CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, appURL);
    CFRelease(appURL);
    if (!bundle) return NULL;
    CFStringRef version = CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleShortVersionString"));
    if (!version) {
        CFRelease(bundle);
        return NULL;
    }
    char buf[128];
    if (!CFStringGetCString(version, buf, sizeof(buf), kCFStringEncodingUTF8)) {
        CFRelease(bundle);
        return NULL;
    }
    CFRelease(bundle);
    return strdup_safe(buf);
}

typedef struct {
    char *version;
} ReleaseInfo;

static bool attr_matches(xmlAttr *attr, const char *prefix, const char *name, const char *value) {
    if (!attr || !attr->name) return false;
    if (strcmp((const char *)attr->name, name) != 0) return false;
    if (prefix) {
        if (!attr->ns || !attr->ns->prefix) return false;
        if (strcmp((const char *)attr->ns->prefix, prefix) != 0) return false;
    }
    xmlChar *val = xmlNodeListGetString(attr->doc, attr->children, 1);
    bool ok = val && strcmp((const char *)val, value) == 0;
    if (val) xmlFree(val);
    return ok;
}

static xmlChar *get_child_content(xmlNode *node, const char *prefix, const char *name) {
    for (xmlNode *c = node->children; c; c = c->next) {
        if (c->type != XML_ELEMENT_NODE || !c->name) continue;
        if (strcmp((const char *)c->name, name) != 0) continue;
        if (prefix) {
            if (!c->ns || !c->ns->prefix) continue;
            if (strcmp((const char *)c->ns->prefix, prefix) != 0) continue;
        }
        return xmlNodeGetContent(c);
    }
    return NULL;
}

static ReleaseInfo parse_feed(const char *path) {
    ReleaseInfo info = {0};
    xmlDoc *doc = xmlReadFile(path, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!doc) return info;
    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root) {
        xmlFreeDoc(doc);
        return info;
    }
    for (xmlNode *channel = root->children; channel; channel = channel->next) {
        if (channel->type != XML_ELEMENT_NODE) continue;
        for (xmlNode *item = channel->children; item; item = item->next) {
            if (item->type != XML_ELEMENT_NODE) continue;
            if (strcmp((const char *)item->name, "item") != 0) continue;

            xmlChar *ver = get_child_content(item, "sparkle", "shortVersionString");
            if (!ver) ver = get_child_content(item, "sparkle", "version");
            if (!ver) {
                xmlChar *title = get_child_content(item, NULL, "title");
                if (title) {
                    char *from_title = extract_version((const char *)title);
                    if (from_title) {
                        info.version = from_title;
                    }
                    xmlFree(title);
                }
            }

            if (ver && !info.version) {
                info.version = strdup_safe((const char *)ver);
                xmlFree(ver);
                break;
            }
            if (ver) xmlFree(ver);
            if (info.version) break;
        }
        if (info.version) break;
    }
    xmlFreeDoc(doc);
    return info;
}

static int download_file(const char *url, const char *out_path) {
    char *argv[] = {"/usr/bin/curl", "-L", "--fail", "--silent", "--show-error", "-o", (char *)out_path, (char *)url, NULL};
    return run_cmd(argv);
}

static int unzip_to(const char *zip_path, const char *dest_dir) {
    char *argv[] = {"/usr/bin/ditto", "-x", "-k", (char *)zip_path, (char *)dest_dir, NULL};
    return run_cmd(argv);
}

static int verify_codesign(const char *app_path) {
    char *argv1[] = {"/usr/bin/codesign", "--verify", "--deep", "--strict", "--verbose=2", (char *)app_path, NULL};
    char *argv2[] = {"/usr/sbin/spctl", "--assess", "--type", "execute", "--verbose=2", (char *)app_path, NULL};
    if (run_cmd(argv1) != 0) return -1;
    if (run_cmd(argv2) != 0) {
        if (getenv("PROJT_BOOTSTRAP_STRICT")) return -1;
        fprintf(stderr, "warning: spctl assessment failed; continuing (set PROJT_BOOTSTRAP_STRICT=1 to enforce).\n");
    }
    return 0;
}

static int install_app(const char *src_app) {
    if (access(INSTALL_PATH, F_OK) == 0) {
        char *rm_argv[] = {"/bin/rm", "-rf", INSTALL_PATH, NULL};
        if (run_cmd(rm_argv) != 0) {
            // best effort; continue
        }
    }

    char *ditto_argv[] = {"/usr/bin/ditto", "--rsrc", (char *)src_app, INSTALL_PATH, NULL};
    if (run_cmd(ditto_argv) == 0) return 0;

    // Try with admin privileges
    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
             "do shell script \"rm -rf '%s'; /usr/bin/ditto --rsrc '%s' '%s'\" with administrator privileges",
             INSTALL_PATH, src_app, INSTALL_PATH);
    char *osa_argv[] = {"/usr/bin/osascript", "-e", cmd, NULL};
    return run_cmd(osa_argv);
}

static int launch_app(void) {
    char *argv[] = {"/usr/bin/open", "-a", "ProjT Launcher", NULL};
    return run_cmd(argv);
}

int main(void) {
    init_ui();
    update_status("Checking for updates...");

    const char *feed_url = FEED_URL;
    if (!feed_url || strlen(feed_url) == 0) {
        show_alert("ProjT Launcher Installer", "Feed URL is not configured.");
        return 1;
    }

    char tmpdir[] = "/tmp/projt-bootstrap-XXXXXX";
    if (!mkdtemp(tmpdir)) {
        show_alert("ProjT Launcher Installer", "Failed to create temp directory.");
        return 1;
    }

    char feed_path[1024];
    snprintf(feed_path, sizeof(feed_path), "%s/feed.xml", tmpdir);
    update_status("Downloading update feed...");
    if (download_file(feed_url, feed_path) != 0) {
        show_alert("ProjT Launcher Installer", "Failed to download update feed.");
        return 1;
    }

    ReleaseInfo info = parse_feed(feed_path);
    if (!info.version) {
        show_alert("ProjT Launcher Installer", "Failed to parse update feed.");
        return 1;
    }

    char *download_url = NULL;
    if (URL_TEMPLATE && strlen(URL_TEMPLATE) > 0) {
        size_t needed = snprintf(NULL, 0, URL_TEMPLATE, info.version, info.version);
        download_url = (char *)malloc(needed + 1);
        if (!download_url) {
            show_alert("ProjT Launcher Installer", "Out of memory while building URL.");
            free(info.version);
            return 1;
        }
        snprintf(download_url, needed + 1, URL_TEMPLATE, info.version, info.version);
    } else {
        show_alert("ProjT Launcher Installer", "Download URL template is not configured.");
        free(info.version);
        return 1;
    }

    char *installed = get_installed_version();
    if (installed && version_compare(installed, info.version) >= 0) {
        update_status("Launching ProjT Launcher...");
        launch_app();
        free(installed);
        free(info.version);
        free(download_url);
        return 0;
    }
    free(installed);

    char zip_path[1024];
    snprintf(zip_path, sizeof(zip_path), "%s/launcher.zip", tmpdir);
    update_status("Downloading latest version...");
    if (download_file(download_url, zip_path) != 0) {
        show_alert("ProjT Launcher Installer", "Failed to download installer zip.");
        free(info.version);
        free(download_url);
        return 1;
    }

    char unpack_dir[1024];
    snprintf(unpack_dir, sizeof(unpack_dir), "%s/unpack", tmpdir);
    mkdir(unpack_dir, 0755);
    update_status("Extracting...");
    if (unzip_to(zip_path, unpack_dir) != 0) {
        show_alert("ProjT Launcher Installer", "Failed to extract installer zip.");
        free(info.version);
        free(download_url);
        return 1;
    }

    char app_path[1024];
    snprintf(app_path, sizeof(app_path), "%s/%s", unpack_dir, APP_NAME);
    if (access(app_path, F_OK) != 0) {
        show_alert("ProjT Launcher Installer", "Downloaded archive did not contain the app bundle.");
        free(info.version);
        free(download_url);
        return 1;
    }

    update_status("Installing to /Applications...");
    if (install_app(app_path) != 0) {
        show_alert("ProjT Launcher Installer", "Failed to install into /Applications.");
        free(info.version);
        free(download_url);
        return 1;
    }

    update_status("Verifying signature...");
    if (verify_codesign(INSTALL_PATH) != 0) {
        show_alert("ProjT Launcher Installer", "Installed app failed signature verification.");
        free(info.version);
        free(download_url);
        return 1;
    }

    update_status("Launching ProjT Launcher...");
    launch_app();
    free(info.version);
    free(download_url);
    return 0;
}
