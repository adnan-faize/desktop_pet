#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdint.h>
#include "../window/window.h"

typedef struct {
    int width, height, channels;
    unsigned char* pixels;
    void* native_image; // Holds XImage* on Linux
} image_t;

image_t* image_load(window_t* win, const char* filename, int scale);
void image_free(image_t* img);

void renderer_draw_image(window_t* win, image_t* img, int sx, int sy, int sw, int sh);
void renderer_present(window_t* win);
void renderer_cleanup(window_t* win);

#endif // RENDERER_H_
