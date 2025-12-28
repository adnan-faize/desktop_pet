/**
 * @file tray.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#include "../platform.h"

#ifdef TRAY_SUPPORT

typedef struct _tray tray;
typedef struct _tray_menu tray_menu;

struct _tray {
    char *icon;
    tray_menu *menu;
};

struct _tray_menu {
    char *text;
    int disabled;
    int checked;

    void (*cb)(tray_menu *);
    void *context;

    tray_menu *submenu;
};

static void tray_update(tray *tray);

#endif
