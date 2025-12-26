/**
 * @file application.c
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#include "application.h"

#include "../platforms/platform.h"

/**
 *
 */
bool application_run(application_t *params)
{
    if (!_app_init()) { return false; }

    while (_app_update()) {}

    if (!_app_exit()) { return false; }

    return true;
}
