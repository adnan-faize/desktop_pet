#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdint.h>
#include "../window/window.h"

typedef struct {
    int width, height;
    int cols, rows;
    int target_w, target_h;
    void** frame_cache; // Array of XImage* (Linux) or similar
} image_t;

image_t* image_load(window_t* win, const char* filename, int cols, int rows, int scale);
void image_free(image_t* img);

void renderer_draw_frame(window_t* win, image_t* img, int frame_index);
void renderer_present(window_t* win);
void renderer_cleanup(window_t* win);

#endif // RENDERER_H_
