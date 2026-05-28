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
    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
        return NULL;
    }

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
    img->width = w;
    img->height = h;
    img->channels = 4;
    img->pixels = data;
    img->native_image = NULL;

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
        if (ximg) {
            ximg->data = NULL;
            XDestroyImage(ximg);
        }
#endif
        stbi_image_free(img->pixels);
        free(img);
    }
}

void renderer_draw_image(window_t* win, image_t* img, int x, int y, int sw, int sh, int sx, int sy) {
#ifdef PLATFORM_LINUX
    Display* display = (Display*)win->display_server;
    Window window = (Window)win->native_handle;
    XImage* ximg = (XImage*)img->native_image;
    
    if (!global_gc) {
        global_gc = XCreateGC(display, window, 0, NULL);
    }

    XClearWindow(display, window);

    if (ximg) {
        // If sw/sh (destination size) is different from source size, we need to scale.
        // For simplicity and performance, if sw is exactly 2x the source frame,
        // we can do a simple nearest-neighbor scale or use XCopyArea with a temporary pixmap.
        // However, XPutImage doesn't scale. We need to scale the pixels or use a Pixmap.
        
        int src_w = img->width / 3; // Hardcoded cols for now to find source frame size
        int src_h = img->height / 4; // Hardcoded rows
        
        if (sw == src_w * 2 && sh == src_h * 2) {
            // Manual nearest neighbor scaling for 2x
            unsigned char* scaled_pixels = malloc(sw * sh * 4);
            for (int i = 0; i < sh; i++) {
                for (int j = 0; j < sw; j++) {
                    int src_i = sy + i / 2;
                    int src_j = sx + j / 2;
                    memcpy(scaled_pixels + (i * sw + j) * 4, 
                           img->pixels + (src_i * img->width + src_j) * 4, 4);
                }
            }
            XImage* xscaled = XCreateImage(display, DefaultVisual(display, DefaultScreen(display)), 
                                          24, ZPixmap, 0, (char*)scaled_pixels, sw, sh, 32, sw * 4);
            // Note: depth/visual should ideally match window, but Default often works for quick tests
            // Re-fetching attributes to be sure:
            XWindowAttributes wa;
            XGetWindowAttributes(display, window, &wa);
            xscaled->format = ZPixmap;
            xscaled->byte_order = LSBFirst;
            xscaled->bitmap_bit_order = LSBFirst;
            
            XPutImage(display, window, global_gc, xscaled, 0, 0, x, y, sw, sh);
            XDestroyImage(xscaled); // Frees scaled_pixels
        } else {
            XPutImage(display, window, global_gc, ximg, sx, sy, x, y, sw, sh);
        }
    }
#endif
}

void renderer_present(window_t* win) {
#ifdef PLATFORM_LINUX
    Display* display = (Display*)win->display_server;
    XFlush(display);
#endif
}
