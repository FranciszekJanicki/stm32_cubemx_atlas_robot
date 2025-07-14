#include "main.h"
#include "FreeRTOS.h"
#include "atlas_hmi.h"
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

    atlas_hmi_config_t config = {};

    atlas_hmi_initialize(&config);
}