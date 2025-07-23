#include "atlas_robot.h"
#include "FreeRTOS.h"
#include "common.h"
#include "hmi_task.h"
#include "kinematics_task.h"
#include "manager.h"
#include "packet_task.h"
#include "semphr.h"
#include "system_task.h"
#include "uart_task.h"
#include "usart.h"

void atlas_robot_initialize(atlas_robot_config_t const* config)
{
    ATLAS_ASSERT(config);

    ATLAS_ERR_CHECK(system_task_initialize(&config->system_ctx));
    ATLAS_ERR_CHECK(uart_task_initialize(&config->uart_ctx));
    ATLAS_ERR_CHECK(hmi_task_initialize(&config->hmi_ctx));
    ATLAS_ERR_CHECK(packet_task_initialize(&config->packet_ctx));
    ATLAS_ERR_CHECK(kinematics_task_initialize(&config->kinematics_ctx));

    vTaskStartScheduler();
}

#undef UART_TASK_NAME
#undef UART_TASK_PRIORITY
#undef UART_TASK_STACK_DEPTH

#undef UART_BUFFER_SIZE

#undef UART_STREAM_BUFFER_STORAGE_SIZE
#undef UART_STREAM_BUFFER_TRIGGER
