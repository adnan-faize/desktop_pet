#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdint.h>

struct _window_t {
    void *native_handle;
    void *display_server;
    uint32_t width;
    uint32_t height;
    int should_close;
};
typedef struct _window_t window_t;

extern window_t* window_create(uint32_t width, uint32_t height, const char* title);
extern void window_poll_events(window_t* win);
extern void window_destroy(window_t* win);
extern void window_set_input_region(window_t* win, int x, int y, int w, int h);
extern void window_get_mouse_pos(window_t* win, int* x, int* y);
extern void window_set_mouse_pos(window_t* win, int x, int y);

// New: Move and Resize the window
extern void window_move(window_t* win, int x, int y);
extern void window_resize(window_t* win, uint32_t w, uint32_t h);

#endif // WINDOW_H_
