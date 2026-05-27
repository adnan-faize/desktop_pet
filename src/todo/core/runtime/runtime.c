/**
 * @file runtime.c
 *
 * @author Adnan FAIZE <adnanfaize@gmail.com>
 */

#include "runtime.h"

typedef bool (*layer_init)(void);

struct _app_layer_t {
    layer_init *inits;
    bool (*layer_update)();
    bool (*layer_exit)();

    void (*send_msg)(char *msg);
    char *(*receive_msg)();
};

bool application_run(application_t *details) {
    return true;
}
