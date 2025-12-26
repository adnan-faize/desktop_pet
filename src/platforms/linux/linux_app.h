/**
 * @file linux_app.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

#include "../platform.h"

#ifdef PLATFORM_LINUX

#include "linux_tray.h"

extern bool __linux_app_init();
extern bool __linux_app_update();
extern bool __linux_app_exit();

#define _app_init() __linux_app_init()
#define _app_update() __linux_app_update()
#define _app_exit() __linux_app_exit()

#endif
