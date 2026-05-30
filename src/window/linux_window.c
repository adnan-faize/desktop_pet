#ifdef PLATFORM_LINUX

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/uinput.h>

#include "window.h"

static int uinput_fd = -1;

static void _window_query_mouse_pos(window_t* win, int* x, int* y) {
    if (!win || !win->display_server) return;
    Display* dpy = (Display*)win->display_server;
    
    XFlush(dpy);
    
    int n_screens = ScreenCount(dpy);
    for (int i = 0; i < n_screens; i++) {
        Window root = RootWindow(dpy, i);
        Window root_return, child_return;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;

        if (XQueryPointer(dpy, root, &root_return, &child_return, 
                         &root_x, &root_y, &win_x, &win_y, &mask)) {
            *x = root_x;
            *y = root_y;
            return;
        }
    }
}

static void uinput_init() {
    uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) return;

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
    if (uinput_fd == -1) uinput_init();

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
    
    // Set window properties for staying on top and skipping taskbar
    Atom state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom atoms[3];
    atoms[0] = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);
    atoms[1] = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    atoms[2] = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", False);
    XChangeProperty(display, window, state, XA_ATOM, 32, PropModeReplace, (unsigned char *)atoms, 3);

    Atom window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom type_utility = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", False);
    XChangeProperty(display, window, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *)&type_utility, 1);

    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display, window);

    window_t* win = malloc(sizeof(window_t));
    win->native_handle = (void*)window;
    win->display_server = (void*)display;
    win->renderer_data = NULL;
    win->width = width;
    win->height = height;
    win->should_close = 0;

    return win;
}

void window_move(window_t* win, int x, int y) {
    Display* display = (Display*)win->display_server;
    Window window = (Window)win->native_handle;
    XMoveWindow(display, window, x, y);
    XFlush(display);
}

void window_resize(window_t* win, uint32_t w, uint32_t h) {
    Display* display = (Display*)win->display_server;
    Window window = (Window)win->native_handle;
    XResizeWindow(display, window, w, h);
    win->width = w;
    win->height = h;
    XFlush(display);
}

void window_get_mouse_pos(window_t* win, int* x, int* y) {
    int dummy_x = 0, dummy_y = 0;
    _window_query_mouse_pos(win, &dummy_x, &dummy_y);
    *x = dummy_x;
    *y = dummy_y;
}

void window_set_mouse_pos(window_t* win, int x, int y) {
    if (uinput_fd < 0) return;
    int cur_x, cur_y;
    window_get_mouse_pos(win, &cur_x, &cur_y);

    struct input_event ev[3];
    memset(ev, 0, sizeof(ev));
    ev[0].type = EV_REL; ev[0].code = REL_X; ev[0].value = x - cur_x;
    ev[1].type = EV_REL; ev[1].code = REL_Y; ev[1].value = y - cur_y;
    ev[2].type = EV_SYN; ev[2].code = SYN_REPORT; ev[2].value = 0;
    write(uinput_fd, ev, sizeof(ev));
}

void window_set_input_region(window_t* win, int x, int y, int w, int h) {
    if (!win || !win->display_server) return;
    Display* dpy = (Display*)win->display_server;
    Window wnd = (Window)win->native_handle;
    
    XserverRegion region;
    if (w > 0 && h > 0) {
        XRectangle rect = { (short)x, (short)y, (unsigned short)w, (unsigned short)h };
        region = XFixesCreateRegion(dpy, &rect, 1);
    } else {
        region = XFixesCreateRegion(dpy, NULL, 0);
    }

    XFixesSetWindowShapeRegion(dpy, wnd, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(dpy, region);
    XFlush(dpy);
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
    if (win && win->display_server) {
        Display* display = (Display*)win->display_server;
        if (win->native_handle) {
            XDestroyWindow(display, (Window)win->native_handle);
        }
        XCloseDisplay(display);
        free(win);
    }

    if (uinput_fd >= 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        uinput_fd = -1;
    }
}

#endif
