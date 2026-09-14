/**
*   @file main.c
*
*   @addtogroup main_module main module documentation
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

#include "drv_gpio.h"
#include "drv_timer.h"

drv_gpio_config_t led_red;
drv_gpio_config_t led_green;
drv_gpio_config_t led_blue;
drv_gpio_config_t button_3;

static void Delay(volatile uint32_t t)
{
    while(t--)
    {
        __asm volatile("nop");
    }
}

void DRV_GPIO_button_3_Callback(drv_gpio_port_t port, uint8_t pin)
{
    if ((port == button_3.port) && (pin == button_3.pin))
    {
        DRV_GPIO_Toggle(led_blue.port, led_blue.pin);
    }
}

int main(void)
{
	led_red.port = DRV_GPIO_PORT_A;
	led_red.pin = 29U;
	led_red.direction = DRV_GPIO_DIRECTION_OUTPUT;
	DRV_GPIO_Init(&led_red);
	DRV_GPIO_Write(led_red.port, led_red.pin, DRV_GPIO_STATE_HIGH);

	led_green.port = DRV_GPIO_PORT_A;
	led_green.pin = 30U;
	led_green.direction = DRV_GPIO_DIRECTION_OUTPUT;
	DRV_GPIO_Init(&led_green);
	DRV_GPIO_Write(led_green.port, led_green.pin, DRV_GPIO_STATE_HIGH);

	led_blue.port = DRV_GPIO_PORT_A;
	led_blue.pin = 31U;
	led_blue.direction = DRV_GPIO_DIRECTION_OUTPUT;
	DRV_GPIO_Init(&led_blue);
	DRV_GPIO_Write(led_blue.port, led_blue.pin, DRV_GPIO_STATE_HIGH);

	button_3.port = DRV_GPIO_PORT_B;
	button_3.pin = 26U;
	button_3.direction = DRV_GPIO_DIRECTION_INPUT;
	DRV_GPIO_Init(&button_3);

	DRV_GPIO_IRQ_Enable(button_3.port, button_3.pin, DRV_GPIO_IRQ_EDGE_RISING, DRV_GPIO_button_3_Callback);

    for(;;)
    {
    	DRV_GPIO_Write(led_red.port, led_red.pin, DRV_GPIO_STATE_LOW);
    	Delay(1000000U);
    	DRV_GPIO_Write(led_red.port, led_red.pin, DRV_GPIO_STATE_HIGH);
    	Delay(1000000U);
    }

    return 1;
}

#ifdef __cplusplus
}
#endif

/** @} */
