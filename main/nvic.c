#include "common.h"
#include "stm32l4xx_hal.h"

extern void hmi_packet_ready_callback(void);
extern void button_press_callback(button_type_t type);
extern void uart_transmit_complete_callback(void);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == 0x00) {
        hmi_packet_ready_callback();
    } else if (GPIO_Pin == 0x01) {
        button_press_callback(BUTTON_TYPE_OK);
    } else if (GPIO_Pin == 0x02) {
        button_press_callback(BUTTON_TYPE_BACK);
    } else if (GPIO_Pin == 0x03) {
        button_press_callback(BUTTON_TYPE_UP);
    } else if (GPIO_Pin == 0x04) {
        button_press_callback(BUTTON_TYPE_DOWN);
    }
}
