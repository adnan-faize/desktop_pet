/**
 * @file 3ds_app.c
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#include "../platform.h"

#ifdef PLATFORM_NINTENDO_3DS

#include <3ds.h>

#include "3ds_app.h"

bool __3ds_app_init() {
    gspInitDefault();
    return true;
}

bool __3ds_app_update() {
    if (!aptMainLoop()) { return false; }

    gspWaitForVBlank();
    gfxSwapBuffers(); // For graphics ?



    return true;
}

bool __3ds_app_exit() {
    gfxExit();
     return true;
}

#endif
