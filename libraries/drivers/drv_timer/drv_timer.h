/**
 * @file    drv_timer.h
 * @brief   Portable Timer Driver interface.
 */

#ifndef DRV_TIMER_H
#define DRV_TIMER_H

/*==============================================================================
 * Includes
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>


/*==============================================================================
 * Public Types
 *============================================================================*/

/**
 * @brief Timer identifiers.
 */
typedef enum
{
    DRV_TIMER_0 = 0,
    DRV_TIMER_1,
    DRV_TIMER_2,
    DRV_TIMER_MAX
} DRV_TIMER_Id_t;


/**
 * @brief Timer operating modes.
 */
typedef enum
{
    DRV_TIMER_MODE_ONE_SHOT = 0,
    DRV_TIMER_MODE_PERIODIC
} DRV_TIMER_Mode_t;


/**
 * @brief Timer configuration structure.
 */
typedef struct
{
    uint32_t period_us;
    DRV_TIMER_Mode_t mode;

} DRV_TIMER_Config_t;


/**
 * @brief Timer callback function type.
 */
typedef void (*DRV_TIMER_Callback_t)(void);


/*==============================================================================
 * Public Function Prototypes
 *============================================================================*/

/**
 * @brief Initializes a timer.
 *
 * @param[in] timer_id Timer identifier.
 * @param[in] config   Pointer to timer configuration.
 *
 * @return true  Timer initialized successfully.
 * @return false Timer initialization failed.
 */
bool DRV_TIMER_Init(DRV_TIMER_Id_t timer_id, const DRV_TIMER_Config_t *config);


/**
 * @brief Starts the selected timer.
 *
 * @param[in] timer_id Timer identifier.
 */
void DRV_TIMER_Start(DRV_TIMER_Id_t timer_id);


/**
 * @brief Stops the selected timer.
 *
 * @param[in] timer_id Timer identifier.
 */
void DRV_TIMER_Stop(DRV_TIMER_Id_t timer_id);


/**
 * @brief Restarts the selected timer.
 *
 * @param[in] timer_id Timer identifier.
 */
void DRV_TIMER_Reset(DRV_TIMER_Id_t timer_id);


/**
 * @brief Gets the current timer counter value.
 *
 * @param[in] timer_id Timer identifier.
 *
 * @return Current timer counter value.
 */
uint32_t DRV_TIMER_GetCount(DRV_TIMER_Id_t timer_id);


/**
 * @brief Checks whether the selected timer period has elapsed.
 *
 * @param[in] timer_id Timer identifier.
 *
 * @return true  Timer period has elapsed.
 * @return false Timer period has not elapsed.
 */
bool DRV_TIMER_IsElapsed(DRV_TIMER_Id_t timer_id);


/**
 * @brief Clears the interrupt flag for the selected timer.
 *
 * @param[in] timer_id Timer identifier.
 */
void DRV_TIMER_ClearFlag(DRV_TIMER_Id_t timer_id);

/**
 * @brief Changes the period of the selected timer.
 *
 * @param[in] timer_id  Timer identifier.
 * @param[in] period_us New timer period in microseconds.
 */
void DRV_TIMER_SetPeriod(DRV_TIMER_Id_t timer_id, uint32_t period_us);

/**
 * @brief Enables the interrupt for the selected timer.
 *
 * @param[in] timer_id Timer identifier.
 */
void DRV_TIMER_IRQ_Enable(DRV_TIMER_Id_t timer_id);


/**
 * @brief Disables the interrupt for the selected timer.
 *
 * @param[in] timer_id Timer identifier.
 */
void DRV_TIMER_IRQ_Disable(DRV_TIMER_Id_t timer_id);


/**
 * @brief Registers a callback for the selected timer.
 *
 * @param[in] timer_id Timer identifier.
 * @param[in] callback Function to execute when the timer interrupt occurs.
 *
 * @return true  Callback registered successfully.
 * @return false Callback registration failed.
 */
bool DRV_TIMER_IRQ_SetCallback(DRV_TIMER_Id_t timer_id,
                               DRV_TIMER_Callback_t callback);


#endif /* DRV_TIMER_H */
