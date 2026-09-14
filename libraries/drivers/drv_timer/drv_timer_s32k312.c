/**
 * @file    drv_timer_s32k312.c
 * @brief   Timer driver implementation for S32K312.
 */

#include "drv_timer.h"
#include "uc_platform_cfg.h"


#if defined(UC_PLATFORM_S32K312)

#include "S32K312.h"
#include <stddef.h>


/*==============================================================================
 * Private Defines
 *============================================================================*/

#define DRV_TIMER_CLOCK_HZ        (48000000UL)
#define DRV_TIMER_US_PER_SECOND   (1000000UL)


/*==============================================================================
 * Private Types
 *============================================================================*/

typedef struct
{
    uint8_t channel;

} drv_timer_hw_config_t;


/*==============================================================================
 * Private Variables
 *============================================================================*/

static const drv_timer_hw_config_t drv_timer_hw[DRV_TIMER_MAX] =
{
    { 0U },
    { 1U },
    { 2U }
};


static DRV_TIMER_Callback_t drv_timer_callback[DRV_TIMER_MAX] =
{
    NULL,
    NULL,
    NULL
};


/*==============================================================================
 * Private Function Prototypes
 *============================================================================*/

static bool drv_timer_is_valid(DRV_TIMER_Id_t timer_id);

static uint32_t drv_timer_period_to_ticks(uint32_t period_us);

void PIT0_IRQHandler(void);

/*==============================================================================
 * Public Functions
 *============================================================================*/

bool DRV_TIMER_Init(DRV_TIMER_Id_t timer_id, const DRV_TIMER_Config_t *config)
{
    bool status = false;
    uint8_t channel;
    uint32_t ticks;

    if ((drv_timer_is_valid(timer_id) == true) && (config != NULL))
    {
        channel = drv_timer_hw[timer_id].channel;

        ticks = drv_timer_period_to_ticks(config->period_us);


        /*
         * Enable PIT module.
         */
        IP_PIT_0->MCR = 0U;


        /*
         * Stop channel before configuration.
         */
        IP_PIT_0->TIMER[channel].TCTRL = 0U;


        /*
         * Load timer period.
         */
        IP_PIT_0->TIMER[channel].LDVAL = ticks - 1UL;


        /*
         * Clear pending timeout flag.
         */
        IP_PIT_0->TIMER[channel].TFLG = PIT_TFLG_TIF_MASK;


        status = true;
    }

    return status;
}


/*----------------------------------------------------------------------------*/

void DRV_TIMER_Start(DRV_TIMER_Id_t timer_id)
{
    uint8_t channel;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        IP_PIT_0->TIMER[channel].TCTRL |= PIT_TCTRL_TEN(1U);
    }
}


/*----------------------------------------------------------------------------*/

void DRV_TIMER_Stop(DRV_TIMER_Id_t timer_id)
{
    uint8_t channel;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        IP_PIT_0->TIMER[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;
    }
}


/*----------------------------------------------------------------------------*/

void DRV_TIMER_Reset(DRV_TIMER_Id_t timer_id)
{
    uint8_t channel;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        IP_PIT_0->TIMER[channel].TFLG = PIT_TFLG_TIF_MASK;

        IP_PIT_0->TIMER[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;

        IP_PIT_0->TIMER[channel].TCTRL |= PIT_TCTRL_TEN(1U);
    }
}


/*----------------------------------------------------------------------------*/

uint32_t DRV_TIMER_GetCount(DRV_TIMER_Id_t timer_id)
{
    uint32_t count = 0UL;
    uint8_t channel;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        count = IP_PIT_0->TIMER[channel].CVAL;
    }

    return count;
}


/*----------------------------------------------------------------------------*/

bool DRV_TIMER_IsElapsed(DRV_TIMER_Id_t timer_id)
{
    bool elapsed = false;
    uint8_t channel;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        if ((IP_PIT_0->TIMER[channel].TFLG & PIT_TFLG_TIF_MASK) != 0U)
        {
            elapsed = true;
        }
    }

    return elapsed;
}


/*----------------------------------------------------------------------------*/

void DRV_TIMER_ClearFlag(DRV_TIMER_Id_t timer_id)
{
    uint8_t channel;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        IP_PIT_0->TIMER[channel].TFLG = PIT_TFLG_TIF_MASK;
    }
}


/*----------------------------------------------------------------------------*/

void DRV_TIMER_SetPeriod(DRV_TIMER_Id_t timer_id, uint32_t period_us)
{
    uint8_t channel;
    uint32_t ticks;

    if ((drv_timer_is_valid(timer_id) == true) && (period_us > 0UL))
    {
        channel = drv_timer_hw[timer_id].channel;
        ticks = drv_timer_period_to_ticks(period_us);

        /*
         * Stop timer.
         */
        IP_PIT_0->TIMER[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;

        /*
         * Load new period.
         */
        IP_PIT_0->TIMER[channel].LDVAL = ticks - 1UL;

        /*
         * Clear previous flag.
         */
        IP_PIT_0->TIMER[channel].TFLG = PIT_TFLG_TIF_MASK;

        /*
         * Restart timer.
         */
        IP_PIT_0->TIMER[channel].TCTRL |= PIT_TCTRL_TEN(1U);
    }
}

/*----------------------------------------------------------------------------*/

void DRV_TIMER_IRQ_Enable(DRV_TIMER_Id_t timer_id)
{
    uint8_t channel;
    uint32_t irq;
    uint32_t reg_index;
    uint32_t bit_index;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        /*
         * Enable PIT channel interrupt.
         */
        IP_PIT_0->TIMER[channel].TCTRL |= PIT_TCTRL_TIE(1U);

        /*
         * Enable PIT0 interrupt in NVIC.
         */
        irq = (uint32_t)PIT0_IRQn;

        reg_index = irq >> 5U;
        bit_index = irq & 0x1FU;

        S32_NVIC->ICPR[reg_index] = (1UL << bit_index);
        S32_NVIC->ISER[reg_index] = (1UL << bit_index);
    }
}


/*----------------------------------------------------------------------------*/

void DRV_TIMER_IRQ_Disable(DRV_TIMER_Id_t timer_id)
{
    uint8_t channel;

    if (drv_timer_is_valid(timer_id) == true)
    {
        channel = drv_timer_hw[timer_id].channel;

        IP_PIT_0->TIMER[channel].TCTRL &= ~PIT_TCTRL_TIE_MASK;
    }
}


/*----------------------------------------------------------------------------*/

bool DRV_TIMER_IRQ_SetCallback(DRV_TIMER_Id_t timer_id, DRV_TIMER_Callback_t callback)
{
    bool status = false;

    if ((drv_timer_is_valid(timer_id) == true) && (callback != NULL))
    {
        drv_timer_callback[timer_id] = callback;

        status = true;
    }

    return status;
}


/*==============================================================================
 * Private Functions
 *============================================================================*/

static bool drv_timer_is_valid(DRV_TIMER_Id_t timer_id)
{
    bool status = false;

    if ((uint32_t)timer_id < (uint32_t)DRV_TIMER_MAX)
    {
        status = true;
    }

    return status;
}


/*----------------------------------------------------------------------------*/

static uint32_t drv_timer_period_to_ticks(uint32_t period_us)
{
    uint32_t ticks;

    ticks = (DRV_TIMER_CLOCK_HZ / DRV_TIMER_US_PER_SECOND) * period_us;

    return ticks;
}


/*==============================================================================
 * Interrupt Service Routine
 *============================================================================*/

void PIT0_IRQHandler(void)
{
    uint8_t channel;

    for (channel = 0U; channel < (uint8_t)DRV_TIMER_MAX; channel++)
    {
        if ((IP_PIT_0->TIMER[channel].TFLG & PIT_TFLG_TIF_MASK) != 0U)
        {
            /*
             * Clear interrupt flag.
             */
            IP_PIT_0->TIMER[channel].TFLG = PIT_TFLG_TIF_MASK;

            /*
             * Execute user callback.
             */
            if (drv_timer_callback[channel] != NULL)
            {
                drv_timer_callback[channel]();
            }
        }
    }
}

#endif /* UC_PLATFORM_S32K312 */
