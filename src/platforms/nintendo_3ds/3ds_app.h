/**
 * @file 3ds_app.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

#include "../platform.h"

#ifdef PLATFORM_NINTENDO_3DS

extern bool __3ds_app_init();
extern bool __3ds_app_update();
extern bool __3ds_app_exit();

#define _app_init() __3ds_app_init()
#define _app_update() __3ds_app_update()
#define _app_exit() __3ds_app_exit()

#endif
