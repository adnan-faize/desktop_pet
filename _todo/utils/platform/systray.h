/**
 * @file systray.h
 *
 * @brief ...
 *
 * @author Adnan FAIZE <adnanfaize@gmail.com>
 */

#ifndef SYSTRAY_H_
#define SYSTRAY_H_

typedef struct _systray_t systray_t;

extern systray_t* systray_create(const char* icon_path, const char* tooltip);

#endif // SYSTRAY_H_
