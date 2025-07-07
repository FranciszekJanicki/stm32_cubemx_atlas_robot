#ifndef BUTTON_TASK_BUTTON_MANAGER_H
#define BUTTON_TASK_BUTTON_MANAGER_H

#include "common.h"

typedef enum {
    BUTTON_NOTIFY_PRESS = ((1 << BUTTON_TYPE_OK) | (1 << BUTTON_TYPE_BACK) |
                           (1 << BUTTON_TYPE_DOWN) | (1 << BUTTON_TYPE_UP)),
    BUTTON_NOTIFY_ALL = (BUTTON_NOTIFY_PRESS),
} button_notify_t;

typedef enum {
    BUTTON_EVENT_TYPE_START,
    BUTTON_EVENT_TYPE_STOP,
} button_event_type_t;

typedef int button_event_payload_start_t;
typedef int button_event_payload_stop_t;

typedef union {
    button_event_payload_start_t start;
    button_event_payload_stop_t stop;
} button_event_payload_t;

typedef struct {
    button_event_type_t type;
    button_event_payload_t payload;
} button_event_t;

typedef struct {
    bool is_running;
} button_manager_t;

atlas_err_t button_manager_process(button_manager_t* manager);
atlas_err_t button_manager_initialize(button_manager_t* manager);

#endif // BUTTON_TASK_BUTTON_MANAGER_H