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

image_t* image_load(window_t* win, const char* filename, int cols, int rows, int scale) {
    int w, h, c;
    unsigned char* data = stbi_load(filename, &w, &h, &c, 4);
    if (!data) return NULL;

    image_t* img = calloc(1, sizeof(image_t));
    if (!img) { stbi_image_free(data); return NULL; }

    int src_frame_w = w / cols;
    int src_frame_h = h / rows;
    img->target_w = src_frame_w * scale;
    img->target_h = src_frame_h * scale;
    img->cols = cols;
    img->rows = rows;
    img->frame_cache = calloc(cols * rows, sizeof(void*));

#ifdef PLATFORM_LINUX
    Display* display = (Display*)win->display_server;
    XWindowAttributes wa;
    XGetWindowAttributes(display, (Window)win->native_handle, &wa);

    for (int f = 0; f < cols * rows; f++) {
        int fx = (f % cols) * src_frame_w;
        int fy = (f / cols) * src_frame_h;

        unsigned char* frame_pixels = malloc(img->target_w * img->target_h * 4);
        if (!frame_pixels) continue;

        for (int i = 0; i < img->target_h; i++) {
            int src_y = fy + (i / scale);
            for (int j = 0; j < img->target_w; j++) {
                int src_x = fx + (j / scale);
                unsigned char r = data[(src_y * w + src_x) * 4 + 0];
                unsigned char g = data[(src_y * w + src_x) * 4 + 1];
                unsigned char b = data[(src_y * w + src_x) * 4 + 2];
                unsigned char a = data[(src_y * w + src_x) * 4 + 3];
                float alpha = a / 255.0f;
                
                frame_pixels[(i * img->target_w + j) * 4 + 0] = (unsigned char)(b * alpha);
                frame_pixels[(i * img->target_w + j) * 4 + 1] = (unsigned char)(g * alpha);
                frame_pixels[(i * img->target_w + j) * 4 + 2] = (unsigned char)(r * alpha);
                frame_pixels[(i * img->target_w + j) * 4 + 3] = a;
            }
        }
        XImage* ximg = XCreateImage(display, wa.visual, wa.depth, ZPixmap, 0, 
                                    (char*)frame_pixels, img->target_w, img->target_h, 32, img->target_w * 4);
        img->frame_cache[f] = (void*)ximg;
    }
#endif

    stbi_image_free(data);
    return img;
}

void image_free(image_t* img) {
    if (img) {
#ifdef PLATFORM_LINUX
        for (int i = 0; i < img->cols * img->rows; i++) {
            XImage* ximg = (XImage*)img->frame_cache[i];
            if (ximg) XDestroyImage(ximg);
        }
#endif
        free(img->frame_cache);
        free(img);
    }
}

void renderer_draw_frame(window_t* win, image_t* img, int frame_index) {
    if (!img || frame_index < 0 || frame_index >= (img->cols * img->rows)) return;
#ifdef PLATFORM_LINUX
    Display* display = (Display*)win->display_server;
    Window window = (Window)win->native_handle;
    if (!global_gc) global_gc = XCreateGC(display, window, 0, NULL);

    XImage* ximg = (XImage*)img->frame_cache[frame_index];
    if (ximg) {
        XPutImage(display, window, global_gc, ximg, 0, 0, 0, 0, img->target_w, img->target_h);
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
