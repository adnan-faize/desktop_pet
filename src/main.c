/**
 *
 *
 * @author Adnan FAIZE <adnanfaize@gmail.com>
 */

#include <stdlib.h>

#ifndef APP_VERSION
#define APP_VERSION "error : unknown version"
#endif

#include "core/features/window.h"

int main(int argc, char** argv) {
    window_t win;
    window_create(&win, 800, 600, "test");

    while (!win.should_close) {
        window_poll_events(&win);
    }

    window_destroy(&win);

    return EXIT_SUCCESS;
}
