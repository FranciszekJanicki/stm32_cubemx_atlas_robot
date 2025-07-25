#ifndef COMMON_NOTIFY_H
#define COMMON_NOTIFY_H

#include "atlas_joint.h"

typedef enum {
    SYSTEM_NOTIFY_PACKET_READY = (1 << 0),
    SYSTEM_NOTIFY_HMI_READY = (1 << 1),
    SYSTEM_NOTIFY_SD_READY = (1 << 2),
    SYSTEM_NOTIFY_KINEMATICS_READY = (1 << 3),
    SYSTEM_NOTIFY_SAFETY_TRIGGER = (1 << 4),
    SYSTEM_NOTIFY_ALL =
        (SYSTEM_NOTIFY_PACKET_READY | SYSTEM_NOTIFY_HMI_READY |
         SYSTEM_NOTIFY_SD_READY | SYSTEM_NOTIFY_KINEMATICS_READY |
         SYSTEM_NOTIFY_SAFETY_TRIGGER),
} system_notify_t;

typedef enum {
    KINEMATICS_NOTIFY_ALL = (1 << 0),
} kinematics_notify_t;

typedef enum {
    BUTTON_NOTIFY_ALL = (1 << 0),
} button_notify_t;

typedef enum {
    DISPLAY_NOTIFY_ALL = (1 << 0),
} display_notify_t;

typedef enum {
    SD_NOTIFY_ALL = (1 << 0),
} sd_notify_t;

typedef enum {
    HMI_NOTIFY_BUTTON_READY = (1 << 0),
    HMI_NOTIFY_DISPLAY_READY = (1 << 1),
    HMI_NOTIFY_ALL = (HMI_NOTIFY_BUTTON_READY | HMI_NOTIFY_DISPLAY_READY),
} hmi_notify_t;

typedef enum {
    PACKET_NOTIFY_ROBOT_PACKET_READY =
        ((1 << ATLAS_JOINT_NUM_1) | (1 << ATLAS_JOINT_NUM_2) |
         (1 << ATLAS_JOINT_NUM_3) | (1 << ATLAS_JOINT_NUM_4) |
         (1 << ATLAS_JOINT_NUM_5) | (1 << ATLAS_JOINT_NUM_6)),
    PACKET_NOTIFY_ALL = (PACKET_NOTIFY_ROBOT_PACKET_READY),
} packet_notify_t;

#endif // COMMON_NOTIFY_H