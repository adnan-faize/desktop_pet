/**
 * @file linux_window.c
 *
 * @brief ...
 *
 * @author Adnan FAIZE <adnanfaize@gmail.com>
 */

#include <stdlib.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../window.h"
#include "X11/X.h"

struct _window_t {
    Window window_hnd;
    Display* display_ptr;
    uint32_t width;
    uint32_t height;
    bool should_close;
};

window_t window_create(uint32_t width, uint32_t height, const char* title) {
    Display* display_ptr = XOpenDisplay(nullptr);
    if (!display_ptr) { return nullptr; }

    int screen = DefaultScreen(display_ptr);

    if (width == 0) { width = DisplayWidth(display_ptr, screen); }
    if (height == 0) { height = DisplayHeight(display_ptr, screen); }

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display_ptr, screen, 32, TrueColor, &vinfo)) {
        XMatchVisualInfo(display_ptr, screen, 24, TrueColor, &vinfo);
    }

    window_t window = malloc(sizeof(struct _window_t));
}

void window_destroy(window_t window) {
    if (window && window->display_ptr) {
        XCloseDisplay(window->display_ptr);
        free(window);
    }
}

bool window_should_close(window_t window) {
    return window->should_close;
}
