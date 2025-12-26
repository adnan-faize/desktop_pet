/**
 * @file win_app.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

#include "../platform.h"

#ifdef PLATFORM_WINDOWS

#include "win_tray.h"

extern bool __win_app_init();
extern bool __win_app_update();
extern bool __win_app_exit();

#define _app_init() __win_app_init()
#define _app_update() __win_app_update()
#define _app_exit() __win_app_exit()

#endif
