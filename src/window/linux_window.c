#ifdef PLATFORM_LINUX

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/uinput.h>

#include "window.h"

static int uinput_fd = -1;

static void uinput_init(uint32_t width, uint32_t height) {
    uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) {
        perror("Warning: Could not open /dev/uinput. Mouse snatching may not work without sudo/permissions.");
        return;
    }

    ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_X);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_Y);

    struct uinput_user_dev uud;
    memset(&uud, 0, sizeof(uud));
    snprintf(uud.name, UINPUT_MAX_NAME_SIZE, "DesktopPet-VirtualMouse");
    uud.id.bustype = BUS_USB;
    uud.id.vendor  = 0x1;
    uud.id.product = 0x1;
    uud.id.version = 1;

    write(uinput_fd, &uud, sizeof(uud));
    ioctl(uinput_fd, UI_DEV_CREATE);
}

window_t* window_create(uint32_t width, uint32_t height, const char* title) {
    Display* display = XOpenDisplay(NULL);
    if (!display) return NULL;

    int screen = DefaultScreen(display);
    
    if (width == 0) width = DisplayWidth(display, screen);
    if (height == 0) height = DisplayHeight(display, screen);

    uinput_init(width, height);

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, screen, 32, TrueColor, &vinfo)) {
        XMatchVisualInfo(display, screen, 24, TrueColor, &vinfo);
    }

    XSetWindowAttributes attrs;
    attrs.colormap = XCreateColormap(display, RootWindow(display, screen), vinfo.visual, AllocNone);
    attrs.border_pixel = 0;
    attrs.background_pixel = 0;
    attrs.override_redirect = True; 

    Window window = XCreateWindow(display, RootWindow(display, screen),
                                0, 0, width, height, 0,
                                vinfo.depth, InputOutput, vinfo.visual,
                                CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attrs);

    XStoreName(display, window, title);
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display, window);

    window_t* win = malloc(sizeof(window_t));
    win->native_handle = (void*)window;
    win->display_server = (void*)display;
    win->width = width;
    win->height = height;
    win->should_close = 0;

    XShapeCombineRectangles(display, window, ShapeInput, 0, 0, NULL, 0, ShapeSet, YXBanded);
    XShapeCombineRectangles(display, window, ShapeBounding, 0, 0, NULL, 0, ShapeSet, YXBanded);

    return win;
}

void window_get_mouse_pos(window_t* win, int* x, int* y) {
    Display* display = (Display*)win->display_server;
    Window root_return, child_return;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;

    if (XQueryPointer(display, RootWindow(display, DefaultScreen(display)), 
                     &root_return, &child_return, &root_x, &root_y, &win_x, &win_y, &mask)) {
        *x = root_x;
        *y = root_y;
    }
}

void window_set_mouse_pos(window_t* win, int x, int y) {
    if (uinput_fd < 0) return;

    int cur_x, cur_y;
    window_get_mouse_pos(win, &cur_x, &cur_y);

    struct input_event ev[3];
    memset(ev, 0, sizeof(ev));

    ev[0].type = EV_REL;
    ev[0].code = REL_X;
    ev[0].value = x - cur_x;

    ev[1].type = EV_REL;
    ev[1].code = REL_Y;
    ev[1].value = y - cur_y;

    ev[2].type = EV_SYN;
    ev[2].code = SYN_REPORT;
    ev[2].value = 0;

    write(uinput_fd, ev, sizeof(ev));
}

void window_set_input_region(window_t* win, int x, int y, int w, int h) {
    Display* display = (Display*)win->display_server;
    Window window = (Window)win->native_handle;
    XRectangle rect = {x, y, w, h};
    XShapeCombineRectangles(display, window, ShapeInput, 0, 0, &rect, 1, ShapeSet, YXBanded);
    XShapeCombineRectangles(display, window, ShapeBounding, 0, 0, &rect, 1, ShapeSet, YXBanded);
    XFlush(display);
}

void window_poll_events(window_t* win) {
    Display* display = (Display*)win->display_server;
    while (display && XPending(display)) {
        XEvent event;
        XNextEvent(display, &event);
        if (event.type == DestroyNotify) win->should_close = 1;
    }
}

void window_destroy(window_t *win) {
    if (uinput_fd >= 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
    }
    if (win && win->display_server) {
        XCloseDisplay((Display*)win->display_server);
        free(win);
    }
}

#endif
