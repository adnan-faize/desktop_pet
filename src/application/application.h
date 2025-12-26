/**
 * @file application.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

typedef struct _application_t application_t;
struct _application_t
{
    char *title;
    char *version;
};

extern bool application_run(application_t *params);
