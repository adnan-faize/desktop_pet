/**
 * @file window.h
 *
 * @brief ...
 *
 * @author Adnan FAIZE <adnanfaize@gmail.com>
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdint.h>

typedef struct _window_t* window_t;

extern window_t window_create(uint32_t width, uint32_t height, const char* title);

extern void window_destroy(window_t window);

extern bool window_should_close(window_t window);

#endif // WINDOW_H_
