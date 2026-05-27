
#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdint.h>

typedef struct {
    void *native_handle;  // Holds HWND on Windows, or Window id on Linux
    void *display_server; // Null on Windows, holds Display* on Linux
    uint32_t width;
    uint32_t height;
    int should_close;
} window_t;

extern int window_create(window_t* win, uint32_t width, uint32_t height, const char* title);

extern void window_poll_events(window_t* win);

extern void window_destroy(window_t* win);

#endif // WINDOW_H_
