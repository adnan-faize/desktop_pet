/**
 * @file application.c
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#include "application.h"

#include "../utils/data-structures/stack/stack.h"

#include "../platforms/platform.h"

struct _app_layer_t {
    bool (*layer_init)();
    bool (*layer_update)();
    bool (*layer_exit)();

    void (*send_msg)(char *msg);
    char *(*receive_msg)();
};

// application layers
// 1. platform
// 2. physics
// 3. rendering
// 4. game
// 5. debug

/**
 *
 */
bool application_run(application_t *params)
{
    if (!_app_init()) { return false; }

    // game init code

    while (_app_update()) {
        // game update code
    }

    // game exit code

    if (!_app_exit()) { return false; }

    return true;
}
