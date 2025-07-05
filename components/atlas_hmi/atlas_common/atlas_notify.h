#ifndef ATLAS_COMMON_ATLAS_NOTIFY_H
#define ATLAS_COMMON_ATLAS_NOTIFY_H

typedef enum {
    ATLAS_SYSTEM_NOTIFY_PACKET_READY = (1 << 0),
    ATLAS_SYSTEM_NOTIFY_HMI_READY = (1 << 1),
    ATLAS_SYSTEM_NOTIFY_SD_READY = (1 << 2),
    ATLAS_SYSTEM_NOTIFY_ALL = (ATLAS_SYSTEM_NOTIFY_PACKET_READY | ATLAS_SYSTEM_NOTIFY_HMI_READY |
                               ATLAS_SYSTEM_NOTIFY_SD_READY),
} atlas_system_notify_t;

typedef enum {
    ATLAS_DISPLAY_NOTIFY_ALL = (1 << 0),
} atlas_display_notify_t;

typedef enum {
    ATLAS_SD_NOTIFY_ALL = (1 << 0),
} atlas_sd_notify_t;

typedef enum {
    ATLAS_PACKET_NOTIFY_PACKET_READY = (1 << 31),
    ATLAS_PACKET_NOTIFY_ALL = (ATLAS_PACKET_NOTIFY_PACKET_READY),
} atlas_packet_notify_t;

#endif // ATLAS_COMMON_ATLAS_NOTIFY_H