/**
 * @file 	drv_gpio_s32k312.c
 * @brief 	GPIO driver implementation for S32K312.
 * @author  Alberto Angel Bautista Niño
 *   		@angel.embeddedsys
 *   		albertobaunino@gmail.com
 *   		abautista.nino@iteso.mx
 */

#include "drv_gpio.h"
#include "uC_platform_cfg.h"

#if defined(UC_PLATFORM_S32K312)

#include "Mcal.h"
#include "S32K312.h"
//#include "core_cm7.h"
#include "nvic.h"

#include <stdint.h>
#include <stddef.h>

/*==============================================================================
 * Private Defines
 *============================================================================*/

/**
 * @brief Number of pins per logical GPIO port.
 *
 * PTA0  -> 0
 * PTA31 -> 31
 * PTB0  -> 32
 * PTB26 -> 58
 */
#define DRV_GPIO_PINS_PER_PORT           (32U)

/**
 * @brief SIUL2 GPDO register block offset.
 */
#define DRV_GPIO_GPDO_OFFSET             (0x1300U)

/**
 * @brief SIUL2 GPDI register block offset.
 */
#define DRV_GPIO_GPDI_OFFSET             (0x1500U)

/**
 * @brief GPIO function selection for MSCR.
 */
#define DRV_GPIO_SSS_GPIO                (0U)


/**
 * @brief Build the SSS field used by S32K312 MSCR/IMCR registers.
 */
#define DRV_GPIO_SIUL2_MSCR_SSS(x)                                  			\
								   (SIUL2_MSCR_SSS_0((x) & 0x01U) |   			\
									SIUL2_MSCR_SSS_1(((x) >> 1U) & 0x01U) | 	\
									SIUL2_MSCR_SSS_2(((x) >> 2U) & 0x01U))


/*
 * ---------------------------------------------------------------------------
 * Known EIRQ routing
 * ---------------------------------------------------------------------------
 *
 * PTB26:
 *
 * Global pad = 32 + 26 = 58
 * EIRQ       = 13
 * IMCR       = 541 - 512 = 29
 * SSS        = 2
 */

#define DRV_GPIO_PTB26_PORT              (1U)
#define DRV_GPIO_PTB26_PIN               (26U)

#define DRV_GPIO_PTB26_EIRQ              (13U)
#define DRV_GPIO_PTB26_IMCR              (29U)
#define DRV_GPIO_PTB26_IMCR_SSS          (2U)


/*==============================================================================
 * Private Function Prototypes
 *============================================================================*/

static uint16_t DRV_GPIO_GetPadIndex(drv_gpio_port_t port, uint8_t pin);
static volatile uint8_t * DRV_GPIO_GetOutputRegister(uint16_t pad);
static volatile const uint8_t * DRV_GPIO_GetInputRegister(uint16_t pad);


/*==============================================================================
 * Private Data
 *============================================================================*/

static drv_gpio_irq_callback_t drv_gpio_eirq13_callback = NULL;


/*==============================================================================
 * Public Functions
 *============================================================================*/

void DRV_GPIO_Init(const drv_gpio_config_t * const config)
{
    uint16_t pad;

    if (config != NULL)
    {
        if (config->pin < DRV_GPIO_PINS_PER_PORT)
        {
            pad = DRV_GPIO_GetPadIndex(config->port, config->pin);

            if (config->direction == DRV_GPIO_DIRECTION_OUTPUT)
            {
                /*
                 * GPIO output:
                 *
                 * SSS = 0 -> GPIO
                 * OBE = 1 -> Output Buffer Enable
                 */
                IP_SIUL2->MSCR[pad] = SIUL2_MSCR_OBE(1U) | DRV_GPIO_SIUL2_MSCR_SSS(DRV_GPIO_SSS_GPIO);
            }
            else
            {
                /*
                 * GPIO input:
                 *
                 * SSS = 0 -> GPIO
                 * IBE = 1 -> Input Buffer Enable
                 */
                IP_SIUL2->MSCR[pad] = SIUL2_MSCR_IBE(1U) | DRV_GPIO_SIUL2_MSCR_SSS(DRV_GPIO_SSS_GPIO);
            }
        }
    }
}


drv_gpio_state_t DRV_GPIO_Read(drv_gpio_port_t port, uint8_t pin)
{
    drv_gpio_state_t state;
    uint16_t pad;
    volatile const uint8_t * input_register;

    state = DRV_GPIO_STATE_LOW;

    if (pin < DRV_GPIO_PINS_PER_PORT)
    {
        pad = DRV_GPIO_GetPadIndex(port, pin);

        input_register = DRV_GPIO_GetInputRegister(pad);

        if ((*input_register & 0x01U) != 0U)
        {
            state = DRV_GPIO_STATE_HIGH;
        }
        else
        {
            state = DRV_GPIO_STATE_LOW;
        }
    }

    return state;
}


void DRV_GPIO_Write(drv_gpio_port_t port, uint8_t pin, drv_gpio_state_t state)
{
    uint16_t pad;
    volatile uint8_t * output_register;

    if (pin < DRV_GPIO_PINS_PER_PORT)
    {
        pad = DRV_GPIO_GetPadIndex(port, pin);

        output_register = DRV_GPIO_GetOutputRegister(pad);

        if (state == DRV_GPIO_STATE_HIGH)
        {
            *output_register = 1U;
        }
        else
        {
            *output_register = 0U;
        }
    }
}


void DRV_GPIO_Toggle(drv_gpio_port_t port, uint8_t pin)
{
    uint16_t pad;
    volatile uint8_t * output_register;

    if (pin < DRV_GPIO_PINS_PER_PORT)
    {
        pad = DRV_GPIO_GetPadIndex(port, pin);

        output_register = DRV_GPIO_GetOutputRegister(pad);

        *output_register ^= 1U;
    }
}


void DRV_GPIO_IRQ_Enable(drv_gpio_port_t port, uint8_t pin, drv_gpio_irq_edge_t edge, drv_gpio_irq_callback_t callback)
{
    /*
     * At this point only PTB26 -> EIRQ13 mapping is
     * validated with the working S32K312 example.
     *
     * Additional GPIO/EIRQ mappings should be added
     * according to S32K312_IOMUX.xlsx.
     */

    if ((port == DRV_GPIO_PTB26_PORT) && (pin == DRV_GPIO_PTB26_PIN))
    {
        /*
         * Configure callback.
         */
    	drv_gpio_eirq13_callback = callback;


        /*
         * PTB26 input configuration.
         */
        IP_SIUL2->MSCR[58U] =
            SIUL2_MSCR_IBE(1U) | DRV_GPIO_SIUL2_MSCR_SSS(DRV_GPIO_SSS_GPIO);

        /*
         * PTB26 -> EIRQ13.
         *
         * IMCR index:
         * 541 - 512 = 29
         *
         * SSS = 2 according to S32K312 IOMUX.
         */
        IP_SIUL2->IMCR[DRV_GPIO_PTB26_IMCR] = DRV_GPIO_SIUL2_MSCR_SSS(DRV_GPIO_PTB26_IMCR_SSS);

        /*
         * Disable both edges first.
         */
        IP_SIUL2->IREER0 &= ~SIUL2_IREER0_IREE13_MASK;

        IP_SIUL2->IFEER0 &= ~SIUL2_IFEER0_IFEE13_MASK;

        /*
         * Configure requested edge.
         */
        if (edge == DRV_GPIO_IRQ_EDGE_RISING)
        {
            IP_SIUL2->IREER0 |= SIUL2_IREER0_IREE13_MASK;
        }
        else if (edge == DRV_GPIO_IRQ_EDGE_FALLING)
        {
            IP_SIUL2->IFEER0 |= SIUL2_IFEER0_IFEE13_MASK;
        }
        else
        {
            /*
             * Both edges.
             */
            IP_SIUL2->IREER0 |= SIUL2_IREER0_IREE13_MASK;

            IP_SIUL2->IFEER0 |= SIUL2_IFEER0_IFEE13_MASK;
        }

        /*
         * Select interrupt request instead of DMA.
         */
        IP_SIUL2->DIRSR0 &= ~SIUL2_DIRSR0_DIRSR13_MASK;

        /*
         * Clear pending EIRQ13 flag.
         */
        IP_SIUL2->DISR0 = SIUL2_DISR0_EIF13_MASK;

        /*
         * Enable EIRQ13.
         */
        IP_SIUL2->DIRER0 |= SIUL2_DIRER0_EIRE13_MASK;

        /*
         * Enable SIUL interrupt in NVIC.
         */
        NVIC_EnableIRQ(SIUL_1_IRQn);

//        __enable_irq();
        /*
         * Enable global interrupts.
         * Cortex-M7: clear PRIMASK.
         */
        __asm volatile ("cpsie i" ::: "memory");
    }
}


void DRV_GPIO_IRQ_Disable(drv_gpio_port_t port, uint8_t pin)
{
    if ((port == DRV_GPIO_PTB26_PORT) && (pin == DRV_GPIO_PTB26_PIN))
    {
        /*
         * Disable rising edge.
         */
        IP_SIUL2->IREER0 &= ~SIUL2_IREER0_IREE13_MASK;

        /*
         * Disable falling edge.
         */
        IP_SIUL2->IFEER0 &= ~SIUL2_IFEER0_IFEE13_MASK;

        /*
         * Disable EIRQ13.
         */
        IP_SIUL2->DIRER0 &= ~SIUL2_DIRER0_EIRE13_MASK;
    }
}


void DRV_GPIO_IRQ_ClearFlag(drv_gpio_port_t port, uint8_t pin)
{
    if ((port == DRV_GPIO_PTB26_PORT) && (pin == DRV_GPIO_PTB26_PIN))
    {
        /*
         * EIRQ flag is cleared by writing 1.
         */
        IP_SIUL2->DISR0 = SIUL2_DISR0_EIF13_MASK;
    }
}
/*==============================================================================
 * Interrupt Service Routines
 *============================================================================*/

#define __INTERRUPT_SIUL_1 __attribute__((interrupt("SIUL1")))
__INTERRUPT_SIUL_1 void SIUL_1_Handler(void)
{
    if ((IP_SIUL2->DISR0 & SIUL2_DISR0_EIF13_MASK) != 0U)
    {
        /*
         * Clear EIRQ13 interrupt flag.
         */
        IP_SIUL2->DISR0 = SIUL2_DISR0_EIF13_MASK;

        /*
         * Execute registered callback.
         */
        if (drv_gpio_eirq13_callback != NULL)
        {
            drv_gpio_eirq13_callback(DRV_GPIO_PORT_B, 26U);
        }
    }
}

/*==============================================================================
 * Private Functions
 *============================================================================*/

static uint16_t DRV_GPIO_GetPadIndex(drv_gpio_port_t port, uint8_t pin)
{
    uint16_t pad;

    pad = ((uint16_t)port * DRV_GPIO_PINS_PER_PORT) + (uint16_t)pin;

    return pad;
}


static volatile uint8_t * DRV_GPIO_GetOutputRegister(uint16_t pad)
{
	uintptr_t register_offset;
    volatile uint8_t * register_ptr;

    register_offset = ((uintptr_t)pad & ~(uintptr_t)0x03U) +
    				  ((uintptr_t)3U - ((uintptr_t)pad & (uintptr_t)0x03U));

    register_ptr = (volatile uint8_t *)
				   ((uintptr_t)IP_SIUL2 + DRV_GPIO_GPDO_OFFSET + register_offset);

    return register_ptr;
}


static volatile const uint8_t * DRV_GPIO_GetInputRegister(uint16_t pad)
{
	uintptr_t register_offset;
    volatile const uint8_t * register_ptr;

    register_offset = ((uintptr_t)pad & ~(uintptr_t)0x03U) +
    				  ((uintptr_t)3U - ((uintptr_t)pad & (uintptr_t)0x03U));

    register_ptr = (volatile const uint8_t *)
				   ((uintptr_t)IP_SIUL2 + DRV_GPIO_GPDI_OFFSET + register_offset);

    return register_ptr;
}


#endif /* UC_PLATFORM_S32K312 */
