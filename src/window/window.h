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

// New: Set mouse position relative to window (screen in full-screen mode)
extern void window_set_mouse_pos(window_t* win, int x, int y);

#endif // WINDOW_H_
