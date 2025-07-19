#include "hmi_manager.h"
#include "FreeRTOS.h"
#include "common.h"
#include "task.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char const* const TAG = "hmi_manager";

static inline bool hmi_manager_has_hmi_event()
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_HMI));
}

static inline bool hmi_manager_send_button_event(button_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_BUTTON),
                      event,
                      pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool hmi_manager_send_display_event(display_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_DISPLAY),
                      event,
                      pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool hmi_manager_send_system_event(system_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_SYSTEM),
                      event,
                      pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool hmi_manager_send_system_notify(system_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_SYSTEM), notify, eSetBits) ==
           pdPASS;
}

static inline bool hmi_manager_receive_hmi_event(hmi_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_HMI),
                         event,
                         pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool hmi_manager_receive_hmi_notify(hmi_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0,
                           HMI_NOTIFY_ALL,
                           (uint32_t*)notify,
                           pdMS_TO_TICKS(1)) == pdPASS;
}

static atlas_err_t hmi_manager_event_start_handler(
    hmi_manager_t* manager,
    hmi_event_payload_start_t const* start)
{
    ATLAS_ASSERT(manager && start);
    ATLAS_LOG_FUNC(TAG);

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    if (manager->is_button_ready) {
        button_event_t event = {.type = BUTTON_EVENT_TYPE_START};
        if (!hmi_manager_send_button_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    if (manager->is_display_ready) {
        display_event_t event = {.type = DISPLAY_EVENT_TYPE_START};
        if (!hmi_manager_send_display_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_event_stop_handler(
    hmi_manager_t* manager,
    hmi_event_payload_stop_t const* stop)
{
    ATLAS_ASSERT(manager && stop);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    {
        button_event_t event = {.type = BUTTON_EVENT_TYPE_START};
        if (!hmi_manager_send_button_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    {
        display_event_t event = {.type = DISPLAY_EVENT_TYPE_START};
        if (!hmi_manager_send_display_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_event_jog_robot_data_handler(
    hmi_manager_t* manager,
    hmi_event_payload_robot_data_t const* robot_data)
{
    ATLAS_ASSERT(manager && robot_data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    system_event_t event = {.origin = SYSTEM_EVENT_ORIGIN_HMI,
                            .type = SYSTEM_EVENT_TYPE_ROBOT_DATA};
    event.payload.robot_data = *robot_data;

    if (!hmi_manager_send_system_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_event_measure_robot_data_handler(
    hmi_manager_t* manager,
    hmi_event_payload_robot_data_t const* robot_data)
{
    ATLAS_ASSERT(manager && robot_data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    display_event_t event = {.type = DISPLAY_EVENT_TYPE_ROBOT_DATA};
    event.payload.robot_data = *robot_data;

    if (!hmi_manager_send_display_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_event_button_data_handler(
    hmi_manager_t* manager,
    hmi_event_payload_button_data_t const* button_data)
{
    ATLAS_ASSERT(manager && button_data);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    ATLAS_LOG(TAG,
              "button type: %s, press: %s, state: %s",
              button_type_to_string(button_data->type),
              button_press_to_string(button_data->press),
              button_state_to_string(button_data->press));

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_event_robot_path_handler(
    hmi_manager_t* manager,
    hmi_event_payload_robot_path_t const* robot_path)
{
    ATLAS_ASSERT(manager && robot_path);
    ATLAS_LOG_FUNC(TAG);

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    display_event_t event = {.type = DISPLAY_EVENT_TYPE_ROBOT_PATH};
    event.payload.robot_path = *robot_path;

    if (!hmi_manager_send_display_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_notify_display_ready_handler(
    hmi_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_display_ready = true;

    if (manager->is_running) {
        display_event_t event = {.type = DISPLAY_EVENT_TYPE_START};
        if (!hmi_manager_send_display_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_notify_button_ready_handler(
    hmi_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_button_ready = true;

    if (manager->is_running) {
        button_event_t event = {.type = BUTTON_EVENT_TYPE_START};
        if (!hmi_manager_send_button_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_notify_handler(hmi_manager_t* manager,
                                              hmi_notify_t notify)
{
    ATLAS_ASSERT(manager);

    if (notify & HMI_NOTIFY_BUTTON_READY) {
        ATLAS_RET_ON_ERR(hmi_manager_notify_button_ready_handler(manager));
    }
    if (notify & HMI_NOTIFY_DISPLAY_READY) {
        ATLAS_RET_ON_ERR(hmi_manager_notify_display_ready_handler(manager));
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t hmi_manager_event_handler(hmi_manager_t* manager,
                                             hmi_event_t const* event)
{
    ATLAS_ASSERT(manager && event);

    switch (event->type) {
        case HMI_EVENT_TYPE_START: {
            return hmi_manager_event_start_handler(manager,
                                                   &event->payload.start);
        }
        case HMI_EVENT_TYPE_STOP: {
            return hmi_manager_event_stop_handler(manager,
                                                  &event->payload.stop);
        }
        case HMI_EVENT_TYPE_ROBOT_DATA: {
            switch (event->origin) {
                case HMI_EVENT_ORIGIN_SYSTEM: {
                    return hmi_manager_event_measure_robot_data_handler(
                        manager,
                        &event->payload.robot_data);
                }
                case HMI_EVENT_ORIGIN_BUTTON: {
                    return hmi_manager_event_jog_robot_data_handler(
                        manager,
                        &event->payload.robot_data);
                }
                default: {
                    return ATLAS_ERR_UNKNOWN_ORIGIN;
                }
            }
        }
        case HMI_EVENT_TYPE_ROBOT_PATH: {
            return hmi_manager_event_robot_path_handler(
                manager,
                &event->payload.robot_path);
        }
        case HMI_EVENT_TYPE_BUTTON_DATA: {
            return hmi_manager_event_button_data_handler(
                manager,
                &event->payload.button_data);
        }
        default: {
            return ATLAS_ERR_UNKNOWN_EVENT;
        }
    }
}

atlas_err_t hmi_manager_process(hmi_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    hmi_notify_t notify;
    if (hmi_manager_receive_hmi_notify(&notify)) {
        ATLAS_RET_ON_ERR(hmi_manager_notify_handler(manager, notify));
    }

    hmi_event_t event;
    while (hmi_manager_has_hmi_event()) {
        if (hmi_manager_receive_hmi_event(&event)) {
            ATLAS_RET_ON_ERR(hmi_manager_event_handler(manager, &event));
        }
    }

    return ATLAS_ERR_OK;
}

atlas_err_t hmi_manager_initialize(hmi_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    manager->is_running = false;
    manager->is_button_ready = false;
    manager->is_display_ready = false;

    return ATLAS_ERR_OK;
}