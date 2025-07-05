#ifndef ATLAS_COMMON_ATLAS_EVENT_H
#define ATLAS_COMMON_ATLAS_EVENT_H

#include "atlas_common.h"
#include "atlas_sd.h"

typedef enum {
    ATLAS_DISPLAY_EVENT_TYPE_START,
    ATLAS_DISPLAY_EVENT_TYPE_STOP,
    ATLAS_DISPLAY_EVENT_TYPE_JOINTS,
    ATLAS_DISPLAY_EVENT_TYPE_CARTESIAN,
} atlas_display_event_type_t;

typedef struct {
} atlas_display_event_payload_start_t;

typedef struct {
} atlas_display_event_payload_stop_t;

typedef struct {
    atlas_joints_data_t data;
} atlas_display_event_payload_joints_t;

typedef struct {
    atlas_cartesian_data_t data;
} atlas_display_event_payload_cartesian_t;

typedef union {
    atlas_display_event_payload_start_t start;
    atlas_display_event_payload_stop_t stop;
    atlas_display_event_payload_joints_t joints;
    atlas_display_event_payload_cartesian_t cartesian;
} atlas_display_event_payload_t;

typedef struct {
    atlas_display_event_type_t type;
    atlas_display_event_payload_t payload;
} atlas_display_event_t;

typedef enum {
    ATLAS_PACKET_EVENT_TYPE_START,
    ATLAS_PACKET_EVENT_TYPE_STOP,
    ATLAS_PACKET_EVENT_TYPE_JOINTS,
    ATLAS_PACKET_EVENT_TYPE_CARTESIAN,
} atlas_packet_event_type_t;

typedef struct {
} atlas_packet_event_payload_start_t;

typedef struct {
} atlas_packet_event_payload_stop_t;

typedef struct {
    atlas_joints_data_t data;
} atlas_packet_event_payload_joints_t;

typedef struct {
    atlas_cartesian_data_t data;
} atlas_packet_payload_cartesian_t;

typedef union {
    atlas_packet_event_payload_start_t start;
    atlas_packet_event_payload_stop_t stop;
    atlas_packet_event_payload_joints_t joints;
    atlas_packet_event_payload_cartesian_t cartesian;
} atlas_packet_event_payload_t;

typedef struct {
    atlas_packet_event_type_t type;
    atlas_packet_event_payload_t payload;
} atlas_packet_event_t;

typedef enum {
    ATLAS_SD_EVENT_TYPE_START,
    ATLAS_SD_EVENT_TYPE_STOP,
    ATLAS_SD_EVENT_TYPE_LS_DIR,
    ATLAS_SD_EVENT_TYPE_CAT_FILE,
    ATLAS_SD_EVENT_TYPE_LOAD_CONFIG,
    ATLAS_SD_EVENT_TYPE_SAVE_CONFIG,
    ATLAS_SD_EVENT_TYPE_LOAD_JOINTS_PATH,
    ATLAS_SD_EVENT_TYPE_SAVE_JOINTS_PATH,
    ATLAS_SD_EVENT_TYPE_LOAD_CARTESIAN_PATH,
    ATLAS_SD_EVENT_TYPE_SAVE_CARTESIAN_PATH,
} atlas_sd_event_type_t;

typedef struct {
} atlas_sd_event_payload_start_t;

typedef struct {
} atlas_sd_event_payload_stop_t;

typedef struct {
    atlas_sd_path_t sd_path;
} atlas_sd_event_payload_ls_dir_t;

typedef struct {
    atlas_sd_path_t sd_path;
} atlas_sd_event_payload_cat_file_t;

typedef struct {
    atlas_sd_path_t sd_path;
} atlas_sd_event_payload_load_config_t;

typedef struct {
    atlas_sd_path_t sd_path;
    atlas_config_t config;
} atlas_sd_event_payload_save_config_t;

typedef struct {
    atlas_sd_path_t sd_path;
} atlas_sd_event_payload_load_cartesian_path_t;

typedef struct {
    atlas_sd_path_t sd_path;
    atlas_cartesian_path_t path;
} atlas_sd_event_payload_save_cartesian_path_t;

typedef struct {
    atlas_sd_path_t sd_path;
} atlas_sd_event_payload_load_atlas_joints_path_t;

typedef struct {
    atlas_sd_path_t sd_path;
    atlas_joints_path_t path;
} atlas_sd_event_payload_save_atlas_joints_path_t;

typedef union {
    atlas_sd_event_payload_start_t start;
    atlas_sd_event_payload_stop_t stop;
    atlas_sd_event_payload_cat_file_t cat_file;
    atlas_sd_event_payload_ls_dir_t ls_dir;
    atlas_sd_event_payload_save_config_t save_config;
    atlas_sd_event_payload_load_config_t load_config;
    atlas_sd_event_payload_save_atlas_joints_path_t save_joints_path;
    atlas_sd_event_payload_load_atlas_joints_path_t load_joints_path;
    atlas_sd_event_payload_save_atlas_cartesian_path_t save_cartesian_path;
    atlas_sd_event_payload_load_atlas_cartesian_path_t load_cartesian_path;
} atlas_sd_event_payload_t;

typedef struct {
    atlas_sd_event_type_t type;
    atlas_sd_event_payload_t payload;
} atlas_sd_event_t;

#endif // ATLAS_COMMON_ATLAS_EVENT_H