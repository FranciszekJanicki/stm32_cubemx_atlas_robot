#include "common.h"
#include "stm32l4xx_hal.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == 0x00) {
        packet_task_robot_packet_ready_callback();
    } else if (GPIO_Pin == 0x01) {
        button_task_press_callback(BUTTON_TYPE_OK);
    } else if (GPIO_Pin == 0x02) {
        button_task_press_callback(BUTTON_TYPE_BACK);
    } else if (GPIO_Pin == 0x03) {
        button_task_press_callback(BUTTON_TYPE_UP);
    } else if (GPIO_Pin == 0x04) {
        button_task_press_callback(BUTTON_TYPE_DOWN);
    }
}

void HAL_UART_TxComplete_Callback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == USART2) {
        uart_task_transmit_complete_callback(task_manager_get(TASK_TYPE_UART));
    }
}
