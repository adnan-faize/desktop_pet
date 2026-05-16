/**
 * @file runtime.h
 *
 * @author Adnan FAIZE <adnanfaize@gmail.com>
 */

#pragma once

typedef enum _application_params_t application_params_t;
enum _application_params_t {
    FULLSCREEN,

};

typedef struct _application_t application_t;
struct _application_t {
    char *title;
    char *version;
    bool (*entrypoint)(void);
    application_params_t *params;
};

extern bool application_run(application_t *details);
