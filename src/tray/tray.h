#ifndef TRAY_H_
#define TRAY_H_

#include <stddef.h>

typedef struct tray_t tray_t;
typedef void (*tray_callback_t)(void* user_data);

tray_t* tray_create(const char* icon_path, const char* tooltip);
void tray_add_item(tray_t* tray, const char* text, tray_callback_t callback, void* user_data);
void tray_update(tray_t* tray);
void tray_destroy(tray_t* tray);

#endif // TRAY_H_
