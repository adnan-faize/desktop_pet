/**
 * @file mac_app.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

#include "../platform.h"

#ifdef PLATFORM_MACOS

#include "mac_tray.h"

extern bool __mac_app_init();
extern bool __mac_app_update();
extern bool __mac_app_exit();

#define _app_init() __mac_app_init()
#define _app_update() __mac_app_update()
#define _app_exit() __mac_app_exit()

#endif
