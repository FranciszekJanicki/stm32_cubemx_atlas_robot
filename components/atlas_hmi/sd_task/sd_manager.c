#include "sd_manager.h"
#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>

static char const* const TAG = "sd_manager";

static inline bool sd_manager_has_sd_event()
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_SD));
}

static inline bool sd_manager_send_system_event(system_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_SYSTEM), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool sd_manager_send_system_notify(system_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_SYSTEM), notify, eSetBits) == pdPASS;
}

static inline bool sd_manager_receive_sd_event(sd_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_PACKET), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool sd_manager_receive_sd_notify(sd_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0, SD_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdPASS;
}

static atlas_err_t sd_manager_notify_handler(sd_manager_t* manager, sd_notify_t notify)
{
    ATLAS_ASSERT(manager);

    return ATLAS_ERR_OK;
}

static atlas_err_t sd_manager_event_start_handler(sd_manager_t* manager,
                                                  sd_event_payload_start_t const* start)
{
    ATLAS_ASSERT(manager && start);
    ATLAS_LOG_FUNC(TAG);

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t sd_manager_event_stop_handler(sd_manager_t* manager,
                                                 sd_event_payload_stop_t const* stop)
{
    ATLAS_ASSERT(manager && stop);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t sd_manager_event_handler(sd_manager_t* manager, sd_event_t const* event)
{
    ATLAS_ASSERT(manager && event);

    switch (event->type) {
        case PACKET_EVENT_TYPE_START:
            return sd_manager_event_start_handler(manager, &event->payload.start);
        case PACKET_EVENT_TYPE_STOP:
            return sd_manager_event_stop_handler(manager, &event->payload.stop);
            return ATLAS_ERR_UNKNOWN_EVENT;
    }
}

atlas_err_t sd_manager_process(sd_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    sd_notify_t notify;
    if (sd_manager_receive_sd_notify(&notify)) {
        ATLAS_RET_ON_ERR(sd_manager_notify_handler(manager, notify));
    }

    sd_event_t event;
    while (sd_manager_has_sd_event()) {
        if (sd_manager_receive_sd_event(&event)) {
            ATLAS_RET_ON_ERR(sd_manager_event_handler(manager, &event));
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t sd_manager_initialize(sd_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_running = false;

    if (!sd_manager_send_system_notify(SYSTEM_NOTIFY_PACKET_READY)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}
