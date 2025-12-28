/**
 * @file win_app.c
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#include "../platform.h"

#ifdef PLATFORM_WINDOWS

bool __app_init(void) {
    return true;
}

bool __app_loop(void) {
    return true;
}

bool __app_exit(void) {
    return true;
}

bool __app_update(void) {
    return true;
}

#endif
