/**
 * @file nds_app.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

#include "../platform.h"

#ifdef PLATFORM_NINTENDO_NDS

extern bool __nds_app_init();
extern bool __nds_app_update();
extern bool __nds_app_exit();

#define _app_init() __nds_app_init()
#define _app_update() __nds_app_update()
#define _app_exit() __nds_app_exit()

#endif
