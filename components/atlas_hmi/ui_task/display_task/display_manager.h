#ifndef DISPLAY_TASK_DISPLAY_MANAGER_H
#define DISPLAY_TASK_DISPLAY_MANAGER_H

typedef enum {
    DISPLAY_EVENT_TYPE_START,
    DISPLAY_EVENT_TYPE_STOP,
    DISPLAY_EVENT_TYPE_JOINTS_DATA,
    DISPLAY_EVENT_TYPE_CARTESIAN_DATA,
} display_event_type_t;

typedef struct {
} display_event_payload_start_t;

typedef struct {
} display_event_payload_stop_t;

typedef struct {
    atlas_joints_data_t data;
} display_event_payload_joints_data_t;

typedef struct {
    atlas_cartesian_data_t data;
} display_event_payload_cartesian_data_t;

typedef union {
    display_event_payload_start_t start;
    display_event_payload_stop_t stop;
    display_event_payload_joints_data_t joints_data;
    display_event_payload_cartesian_data_t cartesian_data;
} display_event_payload_t;

typedef struct {
    display_event_type_t type;
    display_event_payload_t payload;
} display_event_t;

#endif // DISPLAY_TASK_DISPLAY_MANAGER_H