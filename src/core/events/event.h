/**
 * @file events.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#ifndef CORE_EVENT_H_
#define CORE_EVENT_H_

typedef struct _Event *Event;

extern Event event_create(void);

extern bool event_add_callback(void (*callback)(void)); // TODO : if the event can send values, be able to pass them to the callback

#endif // APP_EVENTS_H_
