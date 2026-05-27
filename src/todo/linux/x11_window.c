// invisible window test

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WAYLAND
#include <wayland-client.h>

struct wl_compositor *wl_comp = NULL;
struct wl_surface *wl_surf = NULL;

// Global registry listener to catch compositor and surface bindings
static void registry_handle_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        wl_comp = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    }
}

static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t id) {}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove,
};

int run_wayland_backend(void) {
    // Check if a Wayland display variable actually exists in the environment
    if (!getenv("WAYLAND_DISPlAY")) {
        return 0; // Not a wayland session, drop to fallback
    }

    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        return 0; // Failed to connect, fallback to X11
    }

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    // Roundtrip to let the server bind globals
    wl_display_roundtrip(display);

    if (!wl_comp) {
        wl_display_disconnect(display);
        return 0;
    }

    // Create the window canvas (surface)
    wl_surf = wl_compositor_create_surface(wl_comp);

    // Make it click through : apply empty region boundaries
    struct wl_region *input_region = wl_compositor_create_region(wl_comp);
    wl_surface_set_input_region(wl_surf, input_region);
    wl_surface_commit(wl_surf);
    wl_region_destroy(input_region);

    printf("Native Wayland invisible click-through surface initialized!\n");

    // Basic Wayland event loop processing
    int running = 1;
    while (running && wl_display_dispatch(display) != -1) {
            // App / Engine logic cycles here
    }

    wl_surface_destroy(wl_surf);
    wl_compositor_destroy(wl_comp);
    wl_display_disconnect(display);
    return 1; // Handled successfully
}

#endif

int run_x11_backend(void) {
    // 1. Open connection to the X Server
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Fatal Error: Cannot connect to any display manager (X11/Wayland).\n");
        return 0;
    }
    int screen = DefaultScreen(display);

    // 2. Find a 32-bit visual (Required for Alpha/Transparency)
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, screen, 32, TrueColor, &vinfo)) {
        fprintf(stderr, "No 32-bit visual found. Transparency fallback failed.\n");
        XCloseDisplay(display);
        return 0;
    }

    // 3. Set window attributes
    XSetWindowAttributes attrs;
    attrs.colormap = XCreateColormap(display, RootWindow(display, screen), vinfo.visual, AllocNone);
    attrs.background_pixel = 0x00000000; // Fully transparent alpha
    attrs.border_pixel = 0;

    Window window = XCreateWindow(
        display, RootWindow(display, screen),
        100, 100, 800, 600, 0,
        vinfo.depth, InputOutput, vinfo.visual,
        CWColormap | CWBackPixel | CWBorderPixel, &attrs
    );

    // Bypass window managers on modern Xorg setups to guarantee transparency flags apply
    XSetWindowAttributes set_attrs;
    set_attrs.override_redirect = True;
    XChangeWindowAttributes(display, window, CWOverrideRedirect, &set_attrs);

    // 5. Map (Show) the window FIRST
    XMapWindow(display, window);
    XFlush(display);

    // 4. NOW Make the window Click-Through using XFixes (Ensures target validation)
    XserverRegion region = XFixesCreateRegion(display, NULL, 0);
    XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(display, region);
    XFlush(display);

    printf("Legacy X11 invisible click-through window initialized!\n");

    // 6. Basic Event Loop
    XEvent event;
    int running = 1;
    while (running) {
        XNextEvent(display, &event);
    }

    // Cleanup
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 1;
}

int main(void) {
#ifdef PLATFORM_HAS_WAYLAND
    // Try modern Wayland layout execution first
    if (run_wayland_backend()) {
        return 0;
    }
    printf("Wayland not available. Cascading down to standard X11 runtime...\n");
#endif

    // Fall back completely to old X11 pipeline
    if (run_x11_backend()) {
        return 0;
    }

    return 1;
}
