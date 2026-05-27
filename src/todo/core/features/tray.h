/**
 * @file tray.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#pragma once

/**
 *
 */
typedef struct _tray_t tray_t;

/**
 *
 */
typedef struct _tray_menu_t tray_menu_t;

struct _tray {
    char *icon;
    tray_menu_t *menu;
};

struct _tray_menu_t {
    char *text;

    bool enabled;
    bool checked;

    void (*callback)(tray_menu_t *);
    void *context;

    tray_menu_t *submenu;
};
