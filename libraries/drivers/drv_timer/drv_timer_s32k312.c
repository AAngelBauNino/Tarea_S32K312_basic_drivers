/**
 * @file    drv_timer_s32k312.c
 * @brief   Timer driver implementation for S32K312.
 *
 * @details
 * This file is hardware dependent and shall only be compiled
 * when the S32K312 platform is selected.
 */


/*==============================================================================
 * Includes
 *============================================================================*/

#include "drv_timer.h"
#include "uc_platform_cfg.h"


#if defined(UC_PLATFORM_S32K312)
//#include "Mcal.h"
//#include "S32K312.h"
//
//
///*==============================================================================
// * Private Defines
// *============================================================================*/
//
//#define DRV_TIMER_CLOCK_HZ       (40000000UL)
//#define DRV_TIMER_US_PER_SEC     (1000000UL)
//
//
///*==============================================================================
// * Private Types
// *============================================================================*/
//
///**
// * @brief Hardware mapping information for each portable timer.
// */
//typedef struct
//{
//    uint8_t channel;
//
//} DRV_TIMER_HwConfig_t;
//
//
///*==============================================================================
// * Private Variables
// *============================================================================*/
//
///**
// * @brief Maps portable timer IDs to S32K312 LPIT channels.
// */
//static const DRV_TIMER_HwConfig_t drv_timer_hw[DRV_TIMER_MAX] =
//{
//    { 0U },    /* DRV_TIMER_0 -> LPIT0 Channel 0 */
//    { 1U },    /* DRV_TIMER_1 -> LPIT0 Channel 1 */
//    { 2U }     /* DRV_TIMER_2 -> LPIT0 Channel 2 */
//};
//
//
///**
// * @brief Callback table for timer interrupts.
// */
//static DRV_TIMER_Callback_t drv_timer_callback[DRV_TIMER_MAX] =
//{
//    NULL,
//    NULL,
//    NULL
//};
//
//
///*==============================================================================
// * Private Function Prototypes
// *============================================================================*/
//
//static bool DRV_TIMER_IsValid(DRV_TIMER_Id_t timer_id);
//
//static uint32_t DRV_TIMER_PeriodToTicks(uint32_t period_us);
//
//static IRQn_Type DRV_TIMER_GetIRQn(DRV_TIMER_Id_t timer_id);
//
//
///*==============================================================================
// * Public Functions
// *============================================================================*/
//
//bool DRV_TIMER_Init(DRV_TIMER_Id_t timer_id,
//                    const DRV_TIMER_Config_t *config)
//{
//    bool status = false;
//    uint8_t channel;
//    uint32_t ticks;
//
//    if ((DRV_TIMER_IsValid(timer_id) == true) &&
//        (config != NULL))
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        ticks = DRV_TIMER_PeriodToTicks(config->period_us);
//
//        /*
//         * Enable LPIT module.
//         */
//        LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;
//
//
//        /*
//         * Stop selected channel before configuration.
//         */
//        LPIT0->CLRTEN = (1UL << channel);
//
//
//        /*
//         * Load timer period.
//         */
//        LPIT0->TMR[channel].TVAL = ticks;
//
//
//        /*
//         * Configure timer operation mode.
//         *
//         * LPIT mode bits are MCU-specific and therefore remain
//         * inside this implementation file.
//         */
//        if (config->mode == DRV_TIMER_MODE_PERIODIC)
//        {
//            /*
//             * Periodic mode.
//             *
//             * Default LPIT mode is periodic 32-bit counter.
//             */
//            LPIT0->TMR[channel].TCTRL = 0UL;
//        }
//        else
//        {
//            /*
//             * One-shot implementation can be configured here
//             * according to the desired LPIT mode.
//             */
//            LPIT0->TMR[channel].TCTRL = 0UL;
//        }
//
//
//        /*
//         * Clear pending interrupt flag.
//         */
//        LPIT0->MSR = (1UL << channel);
//
//        status = true;
//    }
//
//    return status;
//}
//
//
///*----------------------------------------------------------------------------*/
//
//void DRV_TIMER_Start(DRV_TIMER_Id_t timer_id)
//{
//    uint8_t channel;
//
//    if (DRV_TIMER_IsValid(timer_id) == true)
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        LPIT0->SETTEN = (1UL << channel);
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
//void DRV_TIMER_Stop(DRV_TIMER_Id_t timer_id)
//{
//    uint8_t channel;
//
//    if (DRV_TIMER_IsValid(timer_id) == true)
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        LPIT0->CLRTEN = (1UL << channel);
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
//void DRV_TIMER_Reset(DRV_TIMER_Id_t timer_id)
//{
//    uint8_t channel;
//
//    if (DRV_TIMER_IsValid(timer_id) == true)
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        LPIT0->CLRTEN = (1UL << channel);
//        LPIT0->SETTEN = (1UL << channel);
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
//uint32_t DRV_TIMER_GetCount(DRV_TIMER_Id_t timer_id)
//{
//    uint32_t count = 0UL;
//    uint8_t channel;
//
//    if (DRV_TIMER_IsValid(timer_id) == true)
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        count = LPIT0->TMR[channel].CVAL;
//    }
//
//    return count;
//}
//
//
///*----------------------------------------------------------------------------*/
//
//void DRV_TIMER_IRQ_Enable(DRV_TIMER_Id_t timer_id)
//{
//    uint8_t channel;
//    IRQn_Type irq_number;
//
//    if (DRV_TIMER_IsValid(timer_id) == true)
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        /*
//         * Enable LPIT interrupt for selected channel.
//         */
//        LPIT0->MIER |= (1UL << channel);
//
//
//        /*
//         * Enable corresponding interrupt at NVIC level.
//         */
//        irq_number = DRV_TIMER_GetIRQn(timer_id);
//
//        NVIC_ClearPendingIRQ(irq_number);
//        NVIC_EnableIRQ(irq_number);
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
//void DRV_TIMER_IRQ_Disable(DRV_TIMER_Id_t timer_id)
//{
//    uint8_t channel;
//    IRQn_Type irq_number;
//
//    if (DRV_TIMER_IsValid(timer_id) == true)
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        /*
//         * Disable LPIT interrupt.
//         */
//        LPIT0->MIER &= ~(1UL << channel);
//
//
//        /*
//         * Disable interrupt at NVIC level.
//         */
//        irq_number = DRV_TIMER_GetIRQn(timer_id);
//
//        NVIC_DisableIRQ(irq_number);
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
//void DRV_TIMER_IRQ_ClearFlag(DRV_TIMER_Id_t timer_id)
//{
//    uint8_t channel;
//
//    if (DRV_TIMER_IsValid(timer_id) == true)
//    {
//        channel = drv_timer_hw[timer_id].channel;
//
//        /*
//         * LPIT interrupt flags are cleared by writing 1.
//         */
//        LPIT0->MSR = (1UL << channel);
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
//bool DRV_TIMER_IRQ_SetCallback(DRV_TIMER_Id_t timer_id,
//                               DRV_TIMER_Callback_t callback)
//{
//    bool status = false;
//
//    if ((DRV_TIMER_IsValid(timer_id) == true) &&
//        (callback != NULL))
//    {
//        drv_timer_callback[timer_id] = callback;
//
//        status = true;
//    }
//
//    return status;
//}
//
//
///*==============================================================================
// * Private Functions
// *============================================================================*/
//
//static bool DRV_TIMER_IsValid(DRV_TIMER_Id_t timer_id)
//{
//    bool status = false;
//
//    if ((uint32_t)timer_id < (uint32_t)DRV_TIMER_MAX)
//    {
//        status = true;
//    }
//
//    return status;
//}
//
//
///*----------------------------------------------------------------------------*/
//
//static uint32_t DRV_TIMER_PeriodToTicks(uint32_t period_us)
//{
//    uint32_t ticks;
//
//    ticks = (DRV_TIMER_CLOCK_HZ / DRV_TIMER_US_PER_SEC) * period_us;
//
//    return ticks;
//}
//
//
///*----------------------------------------------------------------------------*/
//
//static IRQn_Type DRV_TIMER_GetIRQn(DRV_TIMER_Id_t timer_id)
//{
//    IRQn_Type irq_number = LPIT0_Ch0_IRQn;
//
//    switch (timer_id)
//    {
//        case DRV_TIMER_0:
//        {
//            irq_number = LPIT0_Ch0_IRQn;
//            break;
//        }
//
//        case DRV_TIMER_1:
//        {
//            irq_number = LPIT0_Ch1_IRQn;
//            break;
//        }
//
//        case DRV_TIMER_2:
//        {
//            irq_number = LPIT0_Ch2_IRQn;
//            break;
//        }
//
//        default:
//        {
//            /*
//             * Invalid timer IDs are already filtered by
//             * DRV_TIMER_IsValid().
//             */
//            irq_number = LPIT0_Ch0_IRQn;
//            break;
//        }
//    }
//
//    return irq_number;
//}
//
//
///*==============================================================================
// * Interrupt Service Routines
// *============================================================================*/
//
///**
// * @brief LPIT0 Channel 0 interrupt handler.
// */
//void LPIT0_Ch0_IRQHandler(void)
//{
//    DRV_TIMER_IRQ_ClearFlag(DRV_TIMER_0);
//
//    if (drv_timer_callback[DRV_TIMER_0] != NULL)
//    {
//        drv_timer_callback[DRV_TIMER_0]();
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
///**
// * @brief LPIT0 Channel 1 interrupt handler.
// */
//void LPIT0_Ch1_IRQHandler(void)
//{
//    DRV_TIMER_IRQ_ClearFlag(DRV_TIMER_1);
//
//    if (drv_timer_callback[DRV_TIMER_1] != NULL)
//    {
//        drv_timer_callback[DRV_TIMER_1]();
//    }
//}
//
//
///*----------------------------------------------------------------------------*/
//
///**
// * @brief LPIT0 Channel 2 interrupt handler.
// */
//void LPIT0_Ch2_IRQHandler(void)
//{
//    DRV_TIMER_IRQ_ClearFlag(DRV_TIMER_2);
//
//    if (drv_timer_callback[DRV_TIMER_2] != NULL)
//    {
//        drv_timer_callback[DRV_TIMER_2]();
//    }
//}


#endif /* UC_PLATFORM_S32K312 */
