#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PLATFORM_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static GC global_gc = NULL;
#endif

image_t* image_load(window_t* win, const char* filename) {
    int w, h, c;
    unsigned char* data = stbi_load(filename, &w, &h, &c, 4);
    if (!data) return NULL;

    for (int i = 0; i < w * h; i++) {
        float alpha = data[i * 4 + 3] / 255.0f;
        unsigned char r = data[i * 4 + 0];
        unsigned char g = data[i * 4 + 1];
        unsigned char b = data[i * 4 + 2];
        data[i * 4 + 0] = (unsigned char)(b * alpha);
        data[i * 4 + 1] = (unsigned char)(g * alpha);
        data[i * 4 + 2] = (unsigned char)(r * alpha);
    }

    image_t* img = malloc(sizeof(image_t));
    img->width = w; img->height = h; img->channels = 4;
    img->pixels = data; img->native_image = NULL;

#ifdef PLATFORM_LINUX
    Display* display = (Display*)win->display_server;
    XWindowAttributes wa;
    XGetWindowAttributes(display, (Window)win->native_handle, &wa);
    XImage* ximg = XCreateImage(display, wa.visual, wa.depth, ZPixmap, 0, 
                                (char*)img->pixels, img->width, img->height, 32, img->width * 4);
    img->native_image = (void*)ximg;
#endif
    return img;
}

void image_free(image_t* img) {
    if (img) {
#ifdef PLATFORM_LINUX
        XImage* ximg = (XImage*)img->native_image;
        if (ximg) { ximg->data = NULL; XDestroyImage(ximg); }
#endif
        stbi_image_free(img->pixels);
        free(img);
    }
}

void renderer_draw_image(window_t* win, image_t* img, int x, int y, int sw, int sh, int sx, int sy) {
#ifdef PLATFORM_LINUX
    Display* display = (Display*)win->display_server;
    Window window = (Window)win->native_handle;
    if (!global_gc) global_gc = XCreateGC(display, window, 0, NULL);

    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    XClearWindow(display, window);

    int src_frame_w = img->width / 3;
    int src_frame_h = img->height / 4;

    if (sw > src_frame_w) {
        unsigned char* scaled_pixels = malloc(sw * sh * 4);
        for (int i = 0; i < sh; i++) {
            for (int j = 0; j < sw; j++) {
                int src_i = sy + (i * src_frame_h / sh);
                int src_j = sx + (j * src_frame_w / sw);
                memcpy(scaled_pixels + (i * sw + j) * 4, img->pixels + (src_i * img->width + src_j) * 4, 4);
            }
        }
        XImage* xscaled = XCreateImage(display, wa.visual, wa.depth, ZPixmap, 0, 
                                      (char*)scaled_pixels, sw, sh, 32, sw * 4);
        // DRAW AT (0,0) LOCAL
        XPutImage(display, window, global_gc, xscaled, 0, 0, 0, 0, sw, sh);
        XDestroyImage(xscaled);
    } else {
        XImage* ximg = (XImage*)img->native_image;
        if (ximg) XPutImage(display, window, global_gc, ximg, sx, sy, 0, 0, sw, sh);
    }
#endif
}

void renderer_present(window_t* win) {
#ifdef PLATFORM_LINUX
    XFlush((Display*)win->display_server);
#endif
}
