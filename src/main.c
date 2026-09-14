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
#include "S32K312.h"

drv_gpio_config_t led_red;
drv_gpio_config_t led_green;
drv_gpio_config_t led_blue;
drv_gpio_config_t button_3;

DRV_TIMER_Config_t timer_polling_config;
DRV_TIMER_Config_t timer_interrupt_config;


void DRV_GPIO_button_3_Callback(drv_gpio_port_t port, uint8_t pin)
{
	static bool fast_period = false;

    if ((port == button_3.port) && (pin == button_3.pin))
    {
        if (fast_period == false)
        {
            DRV_TIMER_SetPeriod(DRV_TIMER_0, 100000UL);

            fast_period = true;
        }
        else
        {
            DRV_TIMER_SetPeriod(DRV_TIMER_0, 500000UL);

            fast_period = false;
        }
    }
}

void DRV_TIMER_1_Callback(void)
{
    DRV_GPIO_Toggle(led_blue.port, led_blue.pin);
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

	timer_polling_config.period_us = 500000UL;
	timer_polling_config.mode = DRV_TIMER_MODE_PERIODIC;
	DRV_TIMER_Init(DRV_TIMER_0, &timer_polling_config);
	DRV_TIMER_Start(DRV_TIMER_0);

	timer_interrupt_config.period_us = 1000000UL;
	timer_interrupt_config.mode = DRV_TIMER_MODE_PERIODIC;
	DRV_TIMER_Init(DRV_TIMER_1, &timer_interrupt_config);
	DRV_TIMER_IRQ_SetCallback(DRV_TIMER_1, DRV_TIMER_1_Callback);
	DRV_TIMER_IRQ_Enable(DRV_TIMER_1);
	DRV_TIMER_Start(DRV_TIMER_1);

    for(;;)
    {
    	if (DRV_TIMER_IsElapsed(DRV_TIMER_0) == true)
    	{
    		DRV_TIMER_ClearFlag(DRV_TIMER_0);
    		DRV_GPIO_Toggle(led_green.port, led_green.pin);
    	}
    }

    return 1;
}

#ifdef __cplusplus
}
#endif

/** @} */
