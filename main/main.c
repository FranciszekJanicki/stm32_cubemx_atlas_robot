#include "main.h"
#include "FreeRTOS.h"
#include "atlas_robot.h"
#include "gpio.h"
#include "rtc.h"
#include "spi.h"
#include "task.h"
#include "tim.h"
#include "usart.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_RTC_Init();
    MX_TIM2_Init();
    MX_SPI1_Init();

    atlas_robot_config_t config =
        {.system_ctx = {.config = {.delta_timer = &htim2,
                                   .timestamp_rtc = &hrtc}},
         .kinematics_ctx = {.config = {}},
         .packet_ctx =
             {.config = {.packet_spi_bus = &hspi1,
                         .packet_ctxs = {[ATLAS_JOINT_NUM_1] =
                                             {.joint_chip_select_gpio = GPIOC,
                                              .joint_chip_select_pin = 1 << 0,
                                              .joint_packet_ready_gpio = GPIOC,
                                              .joint_packet_ready_pin = 1 << 0},
                                         [ATLAS_JOINT_NUM_2] = {.joint_chip_select_gpio = GPIOC,
                                                                .joint_chip_select_pin = 1 << 1,
                                                                .joint_packet_ready_gpio =
                                                                    GPIOC,
                                                                .joint_packet_ready_pin = 1 << 1},
                                         [ATLAS_JOINT_NUM_3] = {.joint_chip_select_gpio = GPIOC,
                                                                .joint_chip_select_pin = 1 << 2,
                                                                .joint_packet_ready_gpio =
                                                                    GPIOC,
                                                                .joint_packet_ready_pin = 1 << 2},
                                         [ATLAS_JOINT_NUM_4] = {.joint_chip_select_gpio = GPIOC,
                                                                .joint_chip_select_pin = 1 << 3,
                                                                .joint_packet_ready_gpio =
                                                                    GPIOC,
                                                                .joint_packet_ready_pin = 1 << 3},
                                         [ATLAS_JOINT_NUM_5] = {.joint_chip_select_gpio = GPIOC,
                                                                .joint_chip_select_pin = 1 << 4,
                                                                .joint_packet_ready_gpio =
                                                                    GPIOC,
                                                                .joint_packet_ready_pin = 1 << 4},
                                         [ATLAS_JOINT_NUM_6] = {.joint_chip_select_gpio = GPIOC,
                                                                .joint_chip_select_pin = 1 << 5,
                                                                .joint_packet_ready_gpio =
                                                                    GPIOC,
                                                                .joint_packet_ready_pin = 1 << 5}}}},
         .uart_ctx = {.uart_bus = &huart2},
         .hmi_ctx = {.button_ctx = {.config = {}},
                     .display_ctx = {.config = {}}}};

    atlas_robot_initialize(&config);
}
