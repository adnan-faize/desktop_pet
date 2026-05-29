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

image_t* image_load(window_t* win, const char* filename, int scale) {
    int w, h, c;
    unsigned char* data = stbi_load(filename, &w, &h, &c, 4);
    if (!data) return NULL;

    int sw = w * scale;
    int sh = h * scale;
    unsigned char* scaled_data = malloc(sw * sh * 4);
    if (!scaled_data) {
        stbi_image_free(data);
        return NULL;
    }

    unsigned char* dst = scaled_data;

    for (int i = 0; i < sh; i++) {
        int src_i = i / scale;
        for (int j = 0; j < sw; j++) {
            int src_j = j / scale;
            unsigned char r = data[(src_i * w + src_j) * 4 + 0];
            unsigned char g = data[(src_i * w + src_j) * 4 + 1];
            unsigned char b = data[(src_i * w + src_j) * 4 + 2];
            unsigned char a = data[(src_i * w + src_j) * 4 + 3];
            float alpha = a / 255.0f;
            
            // Reverting to the exact byte order that worked before (BGRA)
            dst[(i * sw + j) * 4 + 0] = (unsigned char)(b * alpha);
            dst[(i * sw + j) * 4 + 1] = (unsigned char)(g * alpha);
            dst[(i * sw + j) * 4 + 2] = (unsigned char)(r * alpha);
            dst[(i * sw + j) * 4 + 3] = a;
        }
    }
    stbi_image_free(data);

    image_t* img = malloc(sizeof(image_t));
    if (!img) {
        free(scaled_data);
        return NULL;
    }
    img->width = sw; img->height = sh; img->channels = 4;
    img->pixels = scaled_data; img->native_image = NULL;

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
        free(img->pixels);
        free(img);
    }
}

void renderer_draw_image(window_t* win, image_t* img, int sx, int sy, int sw, int sh) {
#ifdef PLATFORM_LINUX
    Display* display = (Display*)win->display_server;
    Window window = (Window)win->native_handle;
    if (!global_gc) global_gc = XCreateGC(display, window, 0, NULL);

    XClearWindow(display, window);

    XImage* ximg = (XImage*)img->native_image;
    if (ximg) {
        XPutImage(display, window, global_gc, ximg, sx, sy, 0, 0, sw, sh);
    }
#endif
}

void renderer_present(window_t* win) {
#ifdef PLATFORM_LINUX
    XFlush((Display*)win->display_server);
#endif
}

void renderer_cleanup(window_t* win) {
#ifdef PLATFORM_LINUX
    if (global_gc && win && win->display_server) {
        XFreeGC((Display*)win->display_server, global_gc);
        global_gc = NULL;
    }
#endif
}
