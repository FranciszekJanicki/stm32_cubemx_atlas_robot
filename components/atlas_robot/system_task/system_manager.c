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

    return xTaskNotify(task_manager_get(TASK_TYPE_PACKET), notify, eSetBits) ==
           pdPASS;
}

static inline bool system_manager_send_hmi_notify(hmi_notify_t notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotify(task_manager_get(TASK_TYPE_HMI), notify, eSetBits) ==
           pdPASS;
}

static inline bool system_manager_send_sd_notify(sd_notify_t notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotify(task_manager_get(TASK_TYPE_SD), notify, eSetBits) ==
           pdPASS;
}

static inline bool system_manager_send_kinematics_notify(
    kinematics_notify_t notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotify(task_manager_get(TASK_TYPE_KINEMATICS),
                       notify,
                       eSetBits) == pdPASS;
}

static inline bool system_manager_receive_system_notify(system_notify_t* notify)
{
    ATLAS_ASSERT(notify);

    return xTaskNotifyWait(0,
                           SYSTEM_NOTIFY_ALL,
                           (uint32_t*)notify,
                           pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_has_system_event()
{
    return uxQueueMessagesWaiting(queue_manager_get(QUEUE_TYPE_SYSTEM));
}

static inline bool system_manager_send_packet_event(packet_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_PACKET),
                      event,
                      pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_send_hmi_event(hmi_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_HMI),
                      event,
                      pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_send_sd_event(sd_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_SD),
                      event,
                      pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_send_kinematics_event(
    kinematics_event_t const* event)
{
    ATLAS_ASSERT(event);

    return xQueueSend(queue_manager_get(QUEUE_TYPE_KINEMATICS),
                      event,
                      pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_receive_system_event(system_event_t* event)
{
    ATLAS_ASSERT(event);

    return xQueueReceive(queue_manager_get(QUEUE_TYPE_SYSTEM),
                         event,
                         pdMS_TO_TICKS(1)) == pdPASS;
}

static inline bool system_manager_start_delta_timer(system_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    return HAL_TIM_Base_Start_IT(manager->config.delta_timer) == HAL_OK;
}

static inline bool system_manager_stop_delta_timer(system_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    return HAL_TIM_Base_Stop_IT(manager->config.delta_timer) == HAL_OK;
}

static inline bool system_manager_start_packet_ready_timer(
    system_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    return HAL_TIM_Base_Start_IT(manager->config.packet_ready_timer) == HAL_OK;
}

static inline bool system_manager_stop_packet_ready_timer(
    system_manager_t* manager)
{
    ATLAS_ASSERT(manager);

    return HAL_TIM_Base_Stop_IT(manager->config.delta_timer) == HAL_OK;
}

static inline bool system_manager_set_timestamp_rtc_timestamp(
    system_manager_t* manager,
    atlas_timestamp_t const* timestamp)
{
    ATLAS_ASSERT(manager && timestamp);

    RTC_TimeTypeDef rtc_time;

    rtc_time.Hours = timestamp->hour;
    rtc_time.Minutes = timestamp->minute;
    rtc_time.Seconds = timestamp->second;

    if (HAL_RTC_SetTime(manager->config.timestamp_rtc,
                        &rtc_time,
                        RTC_FORMAT_BIN) != HAL_OK) {
        return false;
    }

    RTC_DateTypeDef rtc_date;

    rtc_date.Year = timestamp->year;
    rtc_date.Month = timestamp->month;
    rtc_date.Date = timestamp->day;

    if (HAL_RTC_SetDate(manager->config.timestamp_rtc,
                        &rtc_date,
                        RTC_FORMAT_BIN) != HAL_OK) {
        return false;
    }

    return true;
}

static inline bool system_manager_get_timestamp_rtc_timestamp(
    system_manager_t* manager,
    atlas_timestamp_t* timestamp)
{
    ATLAS_ASSERT(manager && timestamp);

    RTC_TimeTypeDef rtc_time;
    if (HAL_RTC_SetTime(manager->config.timestamp_rtc,
                        &rtc_time,
                        RTC_FORMAT_BIN) != HAL_OK) {
        return false;
    }

    timestamp->hour = rtc_time.Hours;
    timestamp->minute = rtc_time.Minutes;
    timestamp->second = rtc_time.Seconds;

    RTC_DateTypeDef rtc_date;
    if (HAL_RTC_SetDate(manager->config.timestamp_rtc,
                        &rtc_date,
                        RTC_FORMAT_BIN) != HAL_OK) {
        return false;
    }

    timestamp->year = rtc_date.Year;
    timestamp->month = rtc_date.Month;
    timestamp->day = rtc_date.Date;

    return true;
}

static inline bool system_manager_send_all_joints_data(
    atlas_joints_data_t const* data)
{
    ATLAS_ASSERT(data);

    packet_event_t event = {.type = PACKET_EVENT_TYPE_JOINT_DATA};

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        event.payload.joint_data.num = num;
        event.payload.joint_data.data.position = data->positions[num];

        if (!system_manager_send_packet_event(&event)) {
            return false;
        }
    }

    return true;
}

static inline bool system_manager_send_all_joints_start(void)
{
    packet_event_t event = {.type = PACKET_EVENT_TYPE_JOINT_START};

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        event.payload.joint_data.num = num;

        if (!system_manager_send_packet_event(&event)) {
            return false;
        }
    }

    return true;
}

static inline bool system_manager_send_all_joints_stop(void)
{
    packet_event_t event = {.type = PACKET_EVENT_TYPE_JOINT_STOP};

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        event.payload.joint_data.num = num;

        if (!system_manager_send_packet_event(&event)) {
            return false;
        }
    }

    return true;
}

static inline bool system_manager_are_all_joints_ready(
    system_manager_t const* manager)
{
    ATLAS_ASSERT(manager);

    for (uint8_t num = 0U; num < ATLAS_JOINT_NUM; ++num) {
        if (!manager->joint_ctxs[num].is_ready ||
            manager->joint_ctxs[num].has_fault) {
            return false;
        }
    }

    return true;
}

static atlas_err_t system_manager_notify_hmi_ready_handler(
    system_manager_t* manager)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    hmi_event_t event = {.type = HMI_EVENT_TYPE_START};

    if (!system_manager_send_hmi_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_notify_sd_ready_handler(
    system_manager_t* manager)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    sd_event_t event = {.type = SD_EVENT_TYPE_START};

    if (!system_manager_send_sd_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_notify_packet_ready_handler(
    system_manager_t* manager)
{
    ATLAS_ASSERT(manager);
    ATLAS_LOG_FUNC(TAG);

    packet_event_t event = {.type = PACKET_EVENT_TYPE_START};

    if (!system_manager_send_packet_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    HAL_TIM_Base_Start_IT(manager->config.packet_ready_timer);

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_notify_handler(system_manager_t* manager,
                                                 system_notify_t notify)
{
    ATLAS_ASSERT(manager);

    if (notify & SYSTEM_NOTIFY_HMI_READY) {
        ATLAS_RET_ON_ERR(system_manager_notify_hmi_ready_handler(manager));
    }
    if (notify & SYSTEM_NOTIFY_SD_READY) {
        ATLAS_RET_ON_ERR(system_manager_notify_sd_ready_handler(manager));
    }
    if (notify & SYSTEM_NOTIFY_PACKET_READY) {
        ATLAS_RET_ON_ERR(system_manager_notify_packet_ready_handler(manager));
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_joint_data_handler(
    system_manager_t* manager,
    system_event_payload_joint_data_t const* joint_data)
{
    ATLAS_ASSERT(manager && joint_data);
    ATLAS_LOG_FUNC(TAG);

    manager->joint_ctxs[joint_data->num].meas_position =
        joint_data->data.position;

    hmi_event_t event = {.type = HMI_EVENT_TYPE_ROBOT_DATA,
                         .origin = HMI_EVENT_ORIGIN_SYSTEM};
    event.payload.robot_data = (hmi_event_payload_robot_data_t){};

    if (!system_manager_send_hmi_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_joint_fault_handler(
    system_manager_t* manager,
    system_event_payload_joint_fault_t const* joint_fault)
{
    ATLAS_ASSERT(manager && joint_fault);
    ATLAS_LOG_FUNC(TAG);

    manager->joint_ctxs[joint_fault->num].has_fault = true;

    packet_event_t event = {.type = PACKET_EVENT_TYPE_JOINT_STOP};
    event.payload.joint_stop = (packet_event_payload_joint_stop_t){};

    if (!system_manager_send_packet_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_joint_ready_handler(
    system_manager_t* manager,
    system_event_payload_joint_ready_t const* joint_ready)
{
    ATLAS_ASSERT(manager && joint_ready);
    ATLAS_LOG_FUNC(TAG);

    manager->joint_ctxs[joint_ready->num].is_ready = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_jog_robot_data_handler(
    system_manager_t* manager,
    system_event_payload_robot_data_t const* robot_data)
{
    ATLAS_ASSERT(manager && robot_data);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_JOG) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    if (robot_data->type == ATLAS_ROBOT_DATA_TYPE_JOINTS) {
        if (!system_manager_send_all_joints_data(&robot_data->payload.joints)) {
            return ATLAS_ERR_FAIL;
        }
    } else {
        kinematics_event_t event = {.type = KINEMATICS_EVENT_TYPE_ROBOT_DATA};
        event.payload.robot_data = *robot_data;

        if (!system_manager_send_kinematics_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_measure_robot_data_handler(
    system_manager_t* manager,
    system_event_payload_robot_data_t const* robot_data)
{
    ATLAS_ASSERT(manager && robot_data);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_PATH) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    if (robot_data->type == ATLAS_ROBOT_DATA_TYPE_JOINTS) {
        if (atlas_joints_data_is_equal(
                &manager->current_path.points[manager->current_path_index],
                &robot_data->payload.joints)) {
            if (manager->current_path_index + 1U ==
                ATLAS_JOINTS_PATH_MAX_POINTS) {
                manager->state = ATLAS_ROBOT_STATE_IDLE;
                manager->current_path_index = 0U;
            } else {
                ++manager->current_path_index;
            }
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_robot_path_handler(
    system_manager_t* manager,
    system_event_payload_robot_path_t const* robot_path)
{
    ATLAS_ASSERT(manager && robot_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_IDLE) {
        return ATLAS_ERR_FAIL;
    }

    if (robot_path->type == ATLAS_ROBOT_PATH_TYPE_JOINTS) {
        manager->current_path = robot_path->payload.joints;
    } else {
        kinematics_event_t event = {.type = KINEMATICS_EVENT_TYPE_ROBOT_PATH};
        event.payload.robot_path = *robot_path;

        if (!system_manager_send_kinematics_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    hmi_event_t event = {.type = HMI_EVENT_TYPE_ROBOT_PATH};
    event.payload.robot_path = *robot_path;

    if (!system_manager_send_hmi_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_start_path_handler(
    system_manager_t* manager,
    system_event_payload_start_path_t const* start_path)
{
    ATLAS_ASSERT(manager && start_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    ATLAS_LOG(TAG, "starting path");

    if (!system_manager_are_all_joints_ready(manager)) {
        return ATLAS_ERR_FAIL;
    }

    if (!system_manager_send_all_joints_start()) {
        return ATLAS_ERR_FAIL;
    }

    if (!system_manager_start_delta_timer(manager)) {
        return ATLAS_ERR_FAIL;
    }

    manager->state = ATLAS_ROBOT_STATE_PATH;
    manager->current_path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_stop_path_handler(
    system_manager_t* manager,
    system_event_payload_stop_path_t const* stop_path)
{
    ATLAS_ASSERT(manager && stop_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_PATH) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    if (!system_manager_send_all_joints_stop()) {
        return ATLAS_ERR_FAIL;
    }

    if (!system_manager_stop_delta_timer(manager)) {
        return ATLAS_ERR_FAIL;
    }

    manager->state = ATLAS_ROBOT_STATE_IDLE;
    manager->current_path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_start_jog_handler(
    system_manager_t* manager,
    system_event_payload_start_jog_t const* start_jog)
{
    ATLAS_ASSERT(manager && start_jog);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    if (!system_manager_send_all_joints_start()) {
        return ATLAS_ERR_FAIL;
    }

    if (!system_manager_start_delta_timer(manager)) {
        return ATLAS_ERR_FAIL;
    }

    if (!system_manager_start_delta_timer(manager)) {
        return ATLAS_ERR_FAIL;
    }

    manager->state = ATLAS_ROBOT_STATE_JOG;
    manager->current_path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_stop_jog_handler(
    system_manager_t* manager,
    system_event_payload_stop_jog_t const* stop_jog)
{
    ATLAS_ASSERT(manager && stop_jog);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_JOG) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    if (!system_manager_send_all_joints_stop()) {
        return ATLAS_ERR_FAIL;
    }

    if (!system_manager_stop_delta_timer(manager)) {
        return ATLAS_ERR_FAIL;
    }

    manager->state = ATLAS_ROBOT_STATE_IDLE;
    manager->current_path_index = 0U;

    return ATLAS_ERR_OK;
}

static atlas_err_t system_manager_event_save_path_handler(
    system_manager_t* manager,
    system_event_payload_save_path_t const* save_path)
{
    ATLAS_ASSERT(manager && save_path);
    ATLAS_LOG_FUNC(TAG);

    if (manager->state != ATLAS_ROBOT_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    sd_event_t event = {.type = SD_EVENT_TYPE_SAVE_PATH};
    event.payload.save_path = *save_path;

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

    if (manager->state != ATLAS_ROBOT_STATE_IDLE) {
        return ATLAS_ERR_IMPROPER_STATE;
    }

    sd_event_t event = {.type = SD_EVENT_TYPE_LOAD_PATH};
    event.payload.load_path = *load_path;

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
        case SYSTEM_EVENT_TYPE_JOINT_DATA: {
            return system_manager_event_joint_data_handler(
                manager,
                &event->payload.joint_data);
        }
        case SYSTEM_EVENT_TYPE_JOINT_FAULT: {
            return system_manager_event_joint_fault_handler(
                manager,
                &event->payload.joint_fault);
        }
        case SYSTEM_EVENT_TYPE_JOINT_READY: {
            return system_manager_event_joint_ready_handler(
                manager,
                &event->payload.joint_ready);
        }
        case SYSTEM_EVENT_TYPE_ROBOT_DATA: {
            switch (event->origin) {
                case SYSTEM_EVENT_ORIGIN_PACKET: {
                    return system_manager_event_measure_robot_data_handler(
                        manager,
                        &event->payload.robot_data);
                }
                case SYSTEM_EVENT_ORIGIN_HMI: {
                    return system_manager_event_jog_robot_data_handler(
                        manager,
                        &event->payload.robot_data);
                }
                default: {
                    return ATLAS_ERR_UNKNOWN_ORIGIN;
                }
            }
        }
        case SYSTEM_EVENT_TYPE_ROBOT_PATH: {
            return system_manager_event_robot_path_handler(
                manager,
                &event->payload.robot_path);
        }
        case SYSTEM_EVENT_TYPE_START_PATH: {
            return system_manager_event_start_path_handler(
                manager,
                &event->payload.start_path);
        }
        case SYSTEM_EVENT_TYPE_STOP_PATH: {
            return system_manager_event_stop_path_handler(
                manager,
                &event->payload.stop_path);
        }
        case SYSTEM_EVENT_TYPE_START_JOG: {
            return system_manager_event_start_jog_handler(
                manager,
                &event->payload.start_jog);
        }
        case SYSTEM_EVENT_TYPE_STOP_JOG: {
            return system_manager_event_stop_jog_handler(
                manager,
                &event->payload.stop_jog);
        }
        case SYSTEM_EVENT_TYPE_LOAD_PATH: {
            return system_manager_event_load_path_handler(
                manager,
                &event->payload.load_path);
        }
        case SYSTEM_EVENT_TYPE_SAVE_PATH: {
            return system_manager_event_save_path_handler(
                manager,
                &event->payload.save_path);
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

atlas_err_t system_manager_initialize(system_manager_t* manager,
                                      system_config_t const* config)
{
    ATLAS_ASSERT(manager && config);

    memset(&manager->current_path, 0, sizeof(manager->current_path));
    memset(&manager->startup_timestamp, 0, sizeof(manager->startup_timestamp));
    memset(&manager->current_timestamp, 0, sizeof(manager->current_timestamp));

    manager->current_path_index = 0U;
    manager->state = ATLAS_ROBOT_STATE_IDLE;
    manager->config = *config;

    return ATLAS_ERR_OK;
}
