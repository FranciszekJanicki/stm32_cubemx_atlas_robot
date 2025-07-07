#ifndef BUTTON_TASK_BUTTON_MANAGER_H
#define BUTTON_TASK_BUTTON_MANAGER_H

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

#endif // BUTTON_TASK_BUTTON_MANAGER_H