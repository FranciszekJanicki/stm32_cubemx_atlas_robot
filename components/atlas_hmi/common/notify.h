#ifndef COMMON_NOTIFY_H
#define COMMON_NOTIFY_H

typedef enum {
    SYSTEM_NOTIFY_PACKET_READY = (1 << 0),
    SYSTEM_NOTIFY_HMI_READY = (1 << 1),
    SYSTEM_NOTIFY_SD_READY = (1 << 2),
    SYSTEM_NOTIFY_ALL =
        (SYSTEM_NOTIFY_PACKET_READY | SYSTEM_NOTIFY_HMI_READY | SYSTEM_NOTIFY_SD_READY),
} system_notify_t;

typedef enum {
    DISPLAY_NOTIFY_ALL = (1 << 0),
} display_notify_t;

typedef enum {
    SD_NOTIFY_ALL = (1 << 0),
} sd_notify_t;

typedef enum {
    PACKET_NOTIFY_PACKET_READY = (1 << 31),
    PACKET_NOTIFY_ALL = (PACKET_NOTIFY_PACKET_READY),
} packet_notify_t;

#endif // COMMON_NOTIFY_H