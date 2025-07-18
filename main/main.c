#include "main.h"
#include "FreeRTOS.h"
#include "atlas_robot.h"
#include "gpio.h"
#include "spi.h"
#include "task.h"
#include "usart.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_SPI1_Init();

    atlas_robot_config_t config = {
        .packet_ctx = {.config = {.joint_packet_ready_gpio = GPIOA,
                                  .joint_packet_ready_pin = 0x00}},
        .uart_ctx = {.uart = &huart2},
        .hmi_ctx = {.button_ctx = {.config = {}}, .display_ctx = {.config = {}}}};

    atlas_robot_initialize(&config);
}