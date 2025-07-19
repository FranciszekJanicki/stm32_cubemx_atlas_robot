#ifndef COMMON_EVENT_H
#define COMMON_EVENT_H

#include "atlas_core.h"
#include "button.h"
#include "sd.h"

typedef enum {
    SYSTEM_EVENT_ORIGIN_KINEMATICS,
    SYSTEM_EVENT_ORIGIN_PACKET,
    SYSTEM_EVENT_ORIGIN_SAFETY,
    SYSTEM_EVENT_ORIGIN_SD,
    SYSTEM_EVENT_ORIGIN_HMI,
} system_event_origin_t;

typedef enum {
    SYSTEM_EVENT_TYPE_JOINT_READY,
    SYSTEM_EVENT_TYPE_JOINT_FAULT,
    SYSTEM_EVENT_TYPE_JOINT_DATA,
    SYSTEM_EVENT_TYPE_ROBOT_DATA,
    SYSTEM_EVENT_TYPE_ROBOT_PATH,
    SYSTEM_EVENT_TYPE_START_PATH,
    SYSTEM_EVENT_TYPE_STOP_PATH,
    SYSTEM_EVENT_TYPE_START_JOG,
    SYSTEM_EVENT_TYPE_STOP_JOG,
    SYSTEM_EVENT_TYPE_SAVE_PATH,
    SYSTEM_EVENT_TYPE_LOAD_PATH,
} system_event_type_t;

typedef struct {
    atlas_joint_num_t num;
    atlas_joint_data_t data;
} system_event_payload_joint_data_t;
typedef struct {
    atlas_joint_num_t num;
    atlas_joint_ready_t ready;
} system_event_payload_joint_ready_t;
typedef struct {
    atlas_joint_num_t num;
    atlas_joint_fault_t fault;
} system_event_payload_joint_fault_t;
typedef atlas_robot_data_t system_event_payload_robot_data_t;
typedef atlas_robot_path_t system_event_payload_robot_path_t;
typedef sd_save_path_t system_event_payload_save_path_t;
typedef sd_load_path_t system_event_payload_load_path_t;
typedef int system_event_payload_start_path_t;
typedef int system_event_payload_stop_path_t;
typedef int system_event_payload_start_jog_t;
typedef int system_event_payload_stop_jog_t;

typedef union {
    system_event_payload_joint_data_t joint_data;
    system_event_payload_joint_fault_t joint_fault;
    system_event_payload_joint_ready_t joint_ready;
    system_event_payload_robot_data_t robot_data;
    system_event_payload_robot_path_t robot_path;
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
    KINEMATICS_EVENT_TYPE_START,
    KINEMATICS_EVENT_TYPE_STOP,
    KINEMATICS_EVENT_TYPE_ROBOT_DATA,
    KINEMATICS_EVENT_TYPE_ROBOT_PATH,
} kinematics_event_type_t;

typedef int kinematics_event_payload_start_t;
typedef int kinematics_event_payload_stop_t;
typedef atlas_robot_data_t kinematics_event_payload_robot_data_t;
typedef atlas_robot_path_t kinematics_event_payload_robot_path_t;

typedef union {
    kinematics_event_payload_start_t start;
    kinematics_event_payload_stop_t stop;
    kinematics_event_payload_robot_data_t robot_data;
    kinematics_event_payload_robot_path_t robot_path;
} kinematics_event_payload_t;

typedef struct {
    kinematics_event_type_t type;
    kinematics_event_payload_t payload;
} kinematics_event_t;

typedef enum {
    HMI_EVENT_ORIGIN_SYSTEM,
    HMI_EVENT_ORIGIN_BUTTON,
    HMI_EVENT_ORIGIN_DISPLAY,
} hmi_event_origin_t;

typedef enum {
    HMI_EVENT_TYPE_START,
    HMI_EVENT_TYPE_STOP,
    HMI_EVENT_TYPE_ROBOT_DATA,
    HMI_EVENT_TYPE_ROBOT_PATH,
    HMI_EVENT_TYPE_BUTTON_DATA,
} hmi_event_type_t;

typedef int hmi_event_payload_start_t;
typedef int hmi_event_payload_stop_t;
typedef atlas_robot_data_t hmi_event_payload_robot_data_t;
typedef atlas_robot_path_t hmi_event_payload_robot_path_t;
typedef button_data_t hmi_event_payload_button_data_t;

typedef union {
    hmi_event_payload_start_t start;
    hmi_event_payload_stop_t stop;
    hmi_event_payload_robot_data_t robot_data;
    hmi_event_payload_robot_path_t robot_path;
    hmi_event_payload_button_data_t button_data;
} hmi_event_payload_t;

typedef struct {
    hmi_event_origin_t origin;
    hmi_event_type_t type;
    hmi_event_payload_t payload;
} hmi_event_t;

typedef enum {
    PACKET_EVENT_TYPE_START,
    PACKET_EVENT_TYPE_STOP,
    PACKET_EVENT_TYPE_JOINTS_START,
    PACKET_EVENT_TYPE_JOINTS_STOP,
    PACKET_EVENT_TYPE_JOINTS_DATA,
} packet_event_type_t;

typedef int packet_event_payload_start_t;
typedef int packet_event_payload_stop_t;
typedef int packet_event_payload_joints_start_t;
typedef int packet_event_payload_joints_stop_t;
typedef atlas_joints_data_t packet_event_payload_joints_data_t;

typedef union {
    packet_event_payload_start_t start;
    packet_event_payload_stop_t stop;
    packet_event_payload_joints_start_t joints_start;
    packet_event_payload_joints_stop_t joints_stop;
    packet_event_payload_joints_data_t joints_data;
} packet_event_payload_t;

typedef struct {
    packet_event_type_t type;
    packet_event_payload_t payload;
} packet_event_t;

typedef enum {
    BUTTON_EVENT_TYPE_START,
    BUTTON_EVENT_TYPE_STOP,
    BUTTON_EVENT_TYPE_BUTTON_DATA,
} button_event_type_t;

typedef int button_event_payload_start_t;
typedef int button_event_payload_stop_t;
typedef button_data_t button_event_payload_button_data_t;

typedef union {
    button_event_payload_start_t start;
    button_event_payload_stop_t stop;
    button_event_payload_button_data_t button_data;
} button_event_payload_t;

typedef struct {
    button_event_type_t type;
    button_event_payload_t payload;
} button_event_t;

typedef enum {
    DISPLAY_EVENT_TYPE_START,
    DISPLAY_EVENT_TYPE_STOP,
    DISPLAY_EVENT_TYPE_ROBOT_DATA,
    DISPLAY_EVENT_TYPE_ROBOT_PATH,
} display_event_type_t;

typedef int display_event_payload_start_t;
typedef int display_event_payload_stop_t;
typedef atlas_robot_data_t display_event_payload_robot_data_t;
typedef atlas_robot_path_t display_event_payload_robot_path_t;

typedef union {
    display_event_payload_start_t start;
    display_event_payload_stop_t stop;
    display_event_payload_robot_data_t robot_data;
    display_event_payload_robot_path_t robot_path;
} display_event_payload_t;

typedef struct {
    display_event_type_t type;
    display_event_payload_t payload;
} display_event_t;

typedef enum {
    SD_EVENT_TYPE_START,
    SD_EVENT_TYPE_STOP,
    SD_EVENT_TYPE_LOAD_PATH,
    SD_EVENT_TYPE_SAVE_PATH,
} sd_event_type_t;

typedef int sd_event_payload_start_t;
typedef int sd_event_payload_stop_t;
typedef sd_load_path_t sd_event_payload_load_path_t;
typedef sd_save_path_t sd_event_payload_save_path_t;

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