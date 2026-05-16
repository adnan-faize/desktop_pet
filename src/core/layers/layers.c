/**
 * @file layers.c
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#include "layers.h"

typedef bool (*layer_init)(void);
typedef bool (*layer_update)(void);
typedef bool (*layer_exit)(void);

struct layers {
    layer_init *inits;
    layer_update *updates;
    layer_exit *exits;
};
