#ifndef COMMON_EVENT_H
#define COMMON_EVENT_H

#include "common.h"
#include "sd.h"

typedef enum {
    DISPLAY_EVENT_TYPE_START,
    DISPLAY_EVENT_TYPE_STOP,
    DISPLAY_EVENT_TYPE_JOINTS,
    DISPLAY_EVENT_TYPE_CARTESIAN,
} display_event_type_t;

typedef struct {
} display_event_payload_start_t;

typedef struct {
} display_event_payload_stop_t;

typedef struct {
    atlas_joints_data_t data;
} display_event_payload_joints_t;

typedef struct {
    atlas_cartesian_data_t data;
} display_event_payload_cartesian_t;

typedef union {
    display_event_payload_start_t start;
    display_event_payload_stop_t stop;
    display_event_payload_joints_t joints;
    display_event_payload_cartesian_t cartesian;
} display_event_payload_t;

typedef struct {
    display_event_type_t type;
    display_event_payload_t payload;
} display_event_t;

typedef enum {
    PACKET_EVENT_TYPE_START,
    PACKET_EVENT_TYPE_STOP,
    PACKET_EVENT_TYPE_JOINTS,
    PACKET_EVENT_TYPE_CARTESIAN,
} packet_event_type_t;

typedef struct {
} packet_event_payload_start_t;

typedef struct {
} packet_event_payload_stop_t;

typedef struct {
    atlas_joints_data_t data;
} packet_event_payload_joints_t;

typedef struct {
    atlas_cartesian_data_t data;
} packet_payload_cartesian_t;

typedef union {
    packet_event_payload_start_t start;
    packet_event_payload_stop_t stop;
    packet_event_payload_joints_t joints;
    packet_event_payload_cartesian_t cartesian;
} packet_event_payload_t;

typedef struct {
    packet_event_type_t type;
    packet_event_payload_t payload;
} packet_event_t;

typedef enum {
    SD_EVENT_TYPE_START,
    SD_EVENT_TYPE_STOP,
    SD_EVENT_TYPE_LS_DIR,
    SD_EVENT_TYPE_CAT_FILE,
    SD_EVENT_TYPE_LOAD_CONFIG,
    SD_EVENT_TYPE_SAVE_CONFIG,
    SD_EVENT_TYPE_LOAD_JOINTS_PATH,
    SD_EVENT_TYPE_SAVE_JOINTS_PATH,
    SD_EVENT_TYPE_LOAD_CARTESIAN_PATH,
    SD_EVENT_TYPE_SAVE_CARTESIAN_PATH,
} sd_event_type_t;

typedef struct {
} sd_event_payload_start_t;

typedef struct {
} sd_event_payload_stop_t;

typedef struct {
    sd_path_t sd_path;
} sd_event_payload_ls_dir_t;

typedef struct {
    sd_path_t sd_path;
} sd_event_payload_cat_file_t;

typedef struct {
    sd_path_t sd_path;
} sd_event_payload_load_config_t;

typedef struct {
    sd_path_t sd_path;
    atlas_config_t config;
} sd_event_payload_save_config_t;

typedef struct {
    sd_path_t sd_path;
} sd_event_payload_load_cartesian_path_t;

typedef struct {
    sd_path_t sd_path;
    atlas_cartesian_path_t path;
} sd_event_payload_save_cartesian_path_t;

typedef struct {
    sd_path_t sd_path;
} sd_event_payload_load_joints_path_t;

typedef struct {
    sd_path_t sd_path;
    atlas_joints_path_t path;
} sd_event_payload_save_joints_path_t;

typedef union {
    sd_event_payload_start_t start;
    sd_event_payload_stop_t stop;
    sd_event_payload_cat_file_t cat_file;
    sd_event_payload_ls_dir_t ls_dir;
    sd_event_payload_save_config_t save_config;
    sd_event_payload_load_config_t load_config;
    sd_event_payload_save_joints_path_t save_joints_path;
    sd_event_payload_load_joints_path_t load_joints_path;
    sd_event_payload_save_cartesian_path_t save_cartesian_path;
    sd_event_payload_load_cartesian_path_t load_cartesian_path;
} sd_event_payload_t;

typedef struct {
    sd_event_type_t type;
    sd_event_payload_t payload;
} sd_event_t;

#endif // COMMON_EVENT_H