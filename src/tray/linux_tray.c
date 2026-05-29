#define _DEFAULT_SOURCE
#ifdef PLATFORM_LINUX
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tray.h"
#include "logger/logger.h"

struct tray_item_data {
    tray_callback_t callback;
    void* user_data;
};

struct tray_t {
    GtkStatusIcon* icon;
    GtkWidget* menu;
};

static void on_menu_item_activate(GtkMenuItem* item, gpointer user_data) {
    struct tray_item_data* data = (struct tray_item_data*)user_data;
    if (data->callback) {
        data->callback(data->user_data);
    }
}

static void on_tray_icon_popup_menu(GtkStatusIcon* icon, guint button, guint activate_time, gpointer user_data) {
    tray_t* tray = (tray_t*)user_data;
    // Use the dedicated position function for Status Icons
    gtk_menu_popup(GTK_MENU(tray->menu), NULL, NULL, 
                  gtk_status_icon_position_menu, icon, 
                  button, activate_time);
}

static void on_tray_icon_activate(GtkStatusIcon* icon, gpointer user_data) {
    // Also show menu on left click for better usability
    on_tray_icon_popup_menu(icon, 1, gtk_get_current_event_time(), user_data);
}

tray_t* tray_create(const char* icon_path, const char* tooltip) {
    char abs_path[PATH_MAX];
    if (realpath(icon_path, abs_path) == NULL) {
        log_error("Tray: Failed to resolve icon path: %s", icon_path);
        return NULL;
    }
    log_info("Tray: Creating with icon: %s", abs_path);

    tray_t* tray = calloc(1, sizeof(tray_t));
    tray->icon = gtk_status_icon_new_from_file(abs_path);
    
    if (!tray->icon) {
        log_error("Tray: Failed to create GtkStatusIcon");
        free(tray);
        return NULL;
    }
    
    gtk_status_icon_set_tooltip_text(tray->icon, tooltip);
    gtk_status_icon_set_visible(tray->icon, TRUE);

    tray->menu = gtk_menu_new();
    
    // Connect both click and right-click
    g_signal_connect(tray->icon, "activate", G_CALLBACK(on_tray_icon_activate), tray);
    g_signal_connect(tray->icon, "popup-menu", G_CALLBACK(on_tray_icon_popup_menu), tray);

    log_info("Tray: Created successfully");
    return tray;
}

void tray_add_item(tray_t* tray, const char* text, tray_callback_t callback, void* user_data) {
    if (!tray) return;
    GtkWidget* item = gtk_menu_item_new_with_label(text);
    struct tray_item_data* data = malloc(sizeof(struct tray_item_data));
    data->callback = callback;
    data->user_data = user_data;
    
    g_signal_connect(item, "activate", G_CALLBACK(on_menu_item_activate), data);
    // Use object data with a destroyer to prevent leaks
    g_object_set_data_full(G_OBJECT(item), "tray-data", data, free);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(tray->menu), item);
    gtk_widget_show_all(tray->menu);
}

void tray_update(tray_t* tray) {
    if (!tray) return;
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
}

void tray_destroy(tray_t* tray) {
    if (!tray) return;
    g_object_unref(tray->icon);
    gtk_widget_destroy(tray->menu);
    free(tray);
}
#endif
