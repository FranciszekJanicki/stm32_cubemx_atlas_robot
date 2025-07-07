#ifndef DISPLAY_TASK_DISPLAY_MANAGER_H
#define DISPLAY_TASK_DISPLAY_MANAGER_H

#include "common.h"
#include "sh1107.h"

typedef enum {
    DISPLAY_NOTIFY_ALL = (1 << 0),
} display_notify_t;

typedef enum {
    DISPLAY_EVENT_TYPE_START,
    DISPLAY_EVENT_TYPE_STOP,
    DISPLAY_EVENT_TYPE_DATA,
} display_event_type_t;

typedef int display_event_payload_start_t;
typedef int display_event_payload_stop_t;
typedef atlas_data_t display_event_payload_data_t;

typedef union {
    display_event_payload_start_t start;
    display_event_payload_stop_t stop;
    display_event_payload_data_t data;
} display_event_payload_t;

typedef struct {
    display_event_type_t type;
    display_event_payload_t payload;
} display_event_t;

typedef struct {
    bool is_running;
    sh1107_t sh1107;
} display_manager_t;

atlas_err_t display_manager_process(display_manager_t* manager);
atlas_err_t display_manager_initialize(display_manager_t* manager);

#endif // DISPLAY_TASK_DISPLAY_MANAGER_H