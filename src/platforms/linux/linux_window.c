
#ifdef PLATFORM_LINUX

#include <X11/Xlib.h>

#include <stdlib.h>
#include <stdint.h>

#include "src/core/features/window.h"

int window_create(window_t* win, uint32_t width, uint32_t height, const char* title) {
    Display* display = XOpenDisplay(NULL);
    if (!display) { return EXIT_FAILURE; }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    Window window = XCreateSimpleWindow(display, root, 10, 10, width, height, 1, BlackPixel(display, screen), WhitePixel(display, screen));

    XStoreName(display, window, title);
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display, window);

    win->native_handle = (void*)window;
    win->display_server = (void*)display;
    win->width = width;
    win->height = height;
    win->should_close = 0;

    return EXIT_SUCCESS;
}

void window_poll_events(window_t* win) {
    Display* display = (Display*)win->display_server;

    while (XPending(display)) {
        XEvent event;
        XNextEvent(display, &event);

        if (event.type == DestroyNotify) {
            win->should_close = 1;
        }
    }
}

void window_destroy(window_t *win) {
    if (win->display_server) {
        XCloseDisplay((Display*)win->display_server);
    }
}

#endif
