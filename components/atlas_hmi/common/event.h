#ifndef COMMON_EVENT_H
#define COMMON_EVENT_H

#include "atlas_core.h"
#include "button.h"
#include "sd_path.h"

typedef enum {
    SYSTEM_EVENT_ORIGIN_SD,
    SYSTEM_EVENT_ORIGIN_UI,
    SYSTEM_EVENT_ORIGIN_PACKET,
} system_event_origin_t;

typedef enum {
    SYSTEM_EVENT_TYPE_JOG,
    SYSTEM_EVENT_TYPE_DATA,
    SYSTEM_EVENT_TYPE_PATH,
    SYSTEM_EVENT_TYPE_LOAD_PATH,
    SYSTEM_EVENT_TYPE_SAVE_PATH,
    SYSTEM_EVENT_TYPE_START_PATH,
    SYSTEM_EVENT_TYPE_STOP_PATH,
    SYSTEM_EVENT_TYPE_START_JOG,
    SYSTEM_EVENT_TYPE_STOP_JOG,
} system_event_type_t;

typedef atlas_data_t system_event_payload_data_t;
typedef atlas_path_t system_event_payload_path_t;
typedef atlas_jog_t system_event_payload_jog_t;

typedef struct {
    sd_path_t sd_path;
    atlas_path_t path;
} system_event_payload_save_path_t;

typedef struct {
    sd_path_t sd_path;
} system_event_payload_load_path_t;

typedef int system_event_payload_start_path_t;
typedef int system_event_payload_stop_path_t;
typedef int system_event_payload_start_jog_t;
typedef int system_event_payload_stop_jog_t;

typedef union {
    system_event_payload_data_t data;
    system_event_payload_path_t path;
    system_event_payload_jog_t jog;
    system_event_payload_start_jog_t start_jog;
    system_event_payload_stop_jog_t stop_jog;
    system_event_payload_start_path_t start_path;
    system_event_payload_stop_path_t stop_path;
    system_event_payload_load_path_t load_path;
    system_event_payload_save_path_t save_path;
} system_event_payload_t;

typedef struct {
    system_event_type_t type;
    system_event_origin_t origin;
    system_event_payload_t payload;
} system_event_t;

typedef enum {
    UI_EVENT_TYPE_START,
    UI_EVENT_TYPE_STOP,
    UI_EVENT_TYPE_DATA,
} ui_event_type_t;

typedef int ui_event_payload_start_t;
typedef int ui_event_payload_stop_t;
typedef atlas_data_t ui_event_payload_data_t;
typedef button_t ui_event_payload_button_t;

typedef union {
    ui_event_payload_start_t start;
    ui_event_payload_stop_t stop;
    ui_event_payload_data_t data;
    ui_event_payload_button_t button;
} ui_event_payload_t;

typedef struct {
    ui_event_type_t type;
    ui_event_payload_t payload;
} ui_event_t;

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

typedef enum {
    PACKET_EVENT_TYPE_START,
    PACKET_EVENT_TYPE_STOP,
    PACKET_EVENT_TYPE_JOG,
    PACKET_EVENT_TYPE_PATH,
    PACKET_EVENT_TYPE_CONFIG,
    PACKET_EVENT_TYPE_START_PATH,
    PACKET_EVENT_TYPE_STOP_PATH,
    PACKET_EVENT_TYPE_START_JOG,
    PACKET_EVENT_TYPE_STOP_JOG,
} packet_event_type_t;

typedef int packet_event_payload_start_t;
typedef int packet_event_payload_stop_t;
typedef atlas_jog_t packet_event_payload_jog_t;
typedef atlas_path_t packet_event_payload_path_t;
typedef atlas_config_t packet_event_payload_config_t;
typedef int packet_event_payload_start_jog_t;
typedef int packet_event_payload_stop_jog_t;
typedef int packet_event_payload_start_path_t;
typedef int packet_event_payload_stop_path_t;

typedef union {
    packet_event_payload_start_t start;
    packet_event_payload_stop_t stop;
    packet_event_payload_jog_t jog;
    packet_event_payload_path_t path;
    packet_event_payload_config_t config;
    packet_event_payload_start_path_t start_path;
    packet_event_payload_stop_path_t stop_path;
    packet_event_payload_start_jog_t start_jog;
    packet_event_payload_stop_jog_t stop_jog;
} packet_event_payload_t;

typedef struct {
    packet_event_type_t type;
    packet_event_payload_t payload;
} packet_event_t;

typedef enum {
    SD_EVENT_TYPE_START,
    SD_EVENT_TYPE_STOP,
    SD_EVENT_TYPE_LOAD_CONFIG,
    SD_EVENT_TYPE_SAVE_CONFIG,
    SD_EVENT_TYPE_LOAD_PATH,
    SD_EVENT_TYPE_SAVE_PATH,
} sd_event_type_t;

typedef int sd_event_payload_start_t;
typedef int sd_event_payload_stop_t;

typedef struct {
    sd_path_t sd_path;
} sd_event_payload_load_path_t;

typedef struct {
    sd_path_t sd_path;
    atlas_path_t path;
} sd_event_payload_save_path_t;

typedef union {
    sd_event_payload_start_t start;
    sd_event_payload_stop_t stop;
    sd_event_payload_save_path_t save_path;
    sd_event_payload_load_path_t load_path;
} sd_event_payload_t;

typedef struct {
    sd_event_type_t type;
    sd_event_payload_t payload;
} sd_event_t;

#endif // COMMON_EVENT_H