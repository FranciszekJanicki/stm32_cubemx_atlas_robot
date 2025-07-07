#include "system_manager.h"
#include "FreeRTOS.h"
#include "common.h"
#include "queue.h"
#include "task.h"
#include <stdint.h>
#include <string.h>

static char const* const TAG = "system_manager";

static inline bool system_manager_send_packet_notify(packet_notify_t notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotify(task_manager_get(TASK_TYPE_PACKET), notify, eSetBits) == pdPASS;
}

static inline bool system_manager_send_ui_notify(ui_notify_t notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotify(task_manager_get(TASK_TYPE_UI), notify, eSetBits) == pdPASS;
}

static inline bool system_manager_send_sd_notify(sd_notify_t notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotify(task_manager_get(TASK_TYPE_SD), notify, eSetBits) == pdPASS;
}

static inline bool system_manager_send_kinematics_notify(kinematics_notify_t notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotify(task_manager_get(TASK_TYPE_KINEMATICS), notify, eSetBits) == pdPASS;
}

static inline bool system_manager_receive_system_notify(system_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0, SYSTEM_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_has_system_event()
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_SYSTEM));
}

static inline bool system_manager_send_packet_event(packet_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_PACKET), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_send_ui_event(ui_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_UI), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_send_sd_event(sd_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_SD), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_send_kinematics_event(kinematics_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_KINEMATICS), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_receive_system_event(system_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_SYSTEM), event, pdMS_TO_TICKS(1)) == pdPASS;
}

static atlas_err_t system_manager_notify_ui_ready_handler(system_manager_t* manager)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    ui_event_t event = {.type = UI_EVENT_TYPE_START};

    if (!system_manager_send_ui_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_notify_sd_ready_handler(system_manager_t* manager)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    sd_event_t event = {.type = SD_EVENT_TYPE_START};

    if (!system_manager_send_sd_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_notify_packet_ready_handler(system_manager_t* manager)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    packet_event_t event = {.type = PACKET_EVENT_TYPE_START};

    if (!system_manager_send_packet_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_notify_handler(system_manager_t* manager, system_notify_t notify)
{
    ATLAS_ASSERT(manager);

    if (notify & SYSTEM_NOTIFY_UI_READY) {
        ATLAS_RET_ON_ERR(system_manager_notify_ui_ready_handler(manager));
    }
    if (notify & SYSTEM_NOTIFY_SD_READY) {
        ATLAS_RET_ON_ERR(system_manager_notify_sd_ready_handler(manager));
    }
    if (notify & SYSTEM_NOTIFY_PACKET_READY) {
        ATLAS_RET_ON_ERR(system_manager_notify_packet_ready_handler(manager));
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_jog_data_handler(system_manager_t* manager,
                                                         system_event_payload_data_t const* data)
{
    ATLAS_ASSERT(manager && data);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_JOG) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    if (data->type == ATLAS_DATA_TYPE_JOINTS) {
        manager->data = data->payload.joints_data;
    } else {
        kinematics_event_t event = {.type = KINEMATICS_EVENT_TYPE_DATA};
        event.payload.data.type = ATLAS_DATA_TYPE_CARTESIAN;
        event.payload.data.payload.cartesian_data = data->payload.cartesian_data;

        if (!system_manager_send_kinematics_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_measured_data_handler(
    system_manager_t* manager,
    system_event_payload_data_t const* data)
{
    ATLAS_ASSERT(manager && data);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_PATH) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    if (data->type == ATLAS_DATA_TYPE_JOINTS) {
        if (atlas_is_joints_data_equal(&manager->path.points[manager->path_index],
                                       &data->payload.joints_data)) {
            if (manager->path_index + 1U == ATLAS_JOINTS_PATH_MAX_POINTS) {
                manager->state = ATLAS_STATE_IDLE;
                manager->path_index = 0U;
            } else {
                ++manager->path_index;
            }
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_path_handler(system_manager_t* manager,
                                                     system_event_payload_path_t const* path)
{
    ATLAS_ASSERT(manager && path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_IDLE) {
        return ATLAS_ERR_FAIL;
    }

    if (path->type == ATLAS_PATH_TYPE_JOINTS) {
        manager->path = path->payload.joints_path;
    } else {
        kinematics_event_t event = {.type = KINEMATICS_EVENT_TYPE_PATH};
        event.payload.path.type = ATLAS_PATH_TYPE_CARTESIAN;
        event.payload.path.payload.cartesian_path = path->payload.cartesian_path;

        if (!system_manager_send_kinematics_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_start_path_handler(
    system_manager_t* manager,
    system_event_payload_start_path_t const* start_path)
{
    ATLAS_ASSERT(manager && start_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    ATLAS_LOG(TAG, "starting path");

    manager->state = ATLAS_STATE_PATH;
    manager->path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_stop_path_handler(
    system_manager_t* manager,
    system_event_payload_stop_path_t const* stop_path)
{
    ATLAS_ASSERT(manager && stop_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_PATH) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    ATLAS_LOG(TAG, "stopping path");

    manager->state = ATLAS_STATE_IDLE;
    manager->path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_start_jog_handler(
    system_manager_t* manager,
    system_event_payload_start_jog_t const* start_jog)
{
    ATLAS_ASSERT(manager && start_jog);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    ATLAS_LOG(TAG, "starting jog");

    manager->state = ATLAS_STATE_JOG;
    manager->path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_stop_jog_handler(
    system_manager_t* manager,
    system_event_payload_stop_jog_t const* stop_jog)
{
    ATLAS_ASSERT(manager && stop_jog);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_JOG) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    ATLAS_LOG(TAG, "stopping jog");

    manager->state = ATLAS_STATE_IDLE;
    manager->path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_save_path_handler(
    system_manager_t* manager,
    system_event_payload_save_path_t const* save_path)
{
    ATLAS_ASSERT(manager && save_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    sd_event_t event = {.type = SD_EVENT_TYPE_SAVE_PATH};
    event.payload.save_path.path = save_path->path;
    event.payload.save_path.sd_path = save_path->sd_path;

    if (!system_manager_send_sd_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_load_path_handler(
    system_manager_t* manager,
    system_event_payload_load_path_t const* load_path)
{
    ATLAS_ASSERT(manager && load_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    sd_event_t event = {.type = SD_EVENT_TYPE_LOAD_PATH};
    event.payload.load_path.sd_path = load_path->sd_path;

    if (!system_manager_send_sd_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_handler(system_manager_t* manager,
                                                system_event_t const* event)
{
    ATLAS_ASSERT(manager && event);

    switch (event->type) {
        case SYSTEM_EVENT_TYPE_DATA: {
            switch (event->origin) {
                case SYSTEM_EVENT_ORIGIN_PACKET: {
                    system_manager_event_measured_data_handler(manager, &event->payload.data);
                }
                case SYSTEM_EVENT_ORIGIN_UI: {
                    system_manager_event_jog_data_handler(manager, &event->payload.data);
                }
                default: {
                    return ATLAS_ERR_UNKNOWN_ORIGIN;
                }
            }
        }
        case SYSTEM_EVENT_TYPE_PATH: {
            return system_manager_event_path_handler(manager, &event->payload.path);
        }
        case SYSTEM_EVENT_TYPE_START_PATH: {
            return system_manager_event_start_path_handler(manager, &event->payload.start_path);
        }
        case SYSTEM_EVENT_TYPE_STOP_PATH: {
            return system_manager_event_stop_path_handler(manager, &event->payload.stop_path);
        }
        case SYSTEM_EVENT_TYPE_START_JOG: {
            return system_manager_event_start_jog_handler(manager, &event->payload.start_jog);
        }
        case SYSTEM_EVENT_TYPE_STOP_JOG: {
            return system_manager_event_stop_jog_handler(manager, &event->payload.stop_jog);
        }
        case SYSTEM_EVENT_TYPE_LOAD_PATH: {
            return system_manager_event_load_path_handler(manager, &event->payload.load_path);
        }
        case SYSTEM_EVENT_TYPE_SAVE_PATH: {
            return system_manager_event_save_path_handler(manager, &event->payload.save_path);
        }
        default: {
            return ATLAS_ERR_UNKNOWN_EVENT;
        }
    }
}

atlas_err_t system_manager_process(system_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    system_notify_t notify;
    if (system_manager_receive_system_notify(&notify)) {
        ATLAS_RET_ON_ERR(system_manager_notify_handler(manager, notify));
    }

    system_event_t event;
    while (system_manager_has_system_event()) {
        if (system_manager_receive_system_event(&event)) {
            ATLAS_RET_ON_ERR(system_manager_event_handler(manager, &event));
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t system_manager_initialize(system_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    memset(&manager->path, 0, sizeof(manager->path));
    memset(&manager->data, 0, sizeof(manager->data));

    manager->state = ATLAS_STATE_IDLE;
    manager->timestamp = 0U;
    manager->path_index = 0U;

    return ATLAS_ERR_OK;
}
