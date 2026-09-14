/**
 * @file 	drv_gpio.h
 * @brief 	Portable GPIO driver interface.
 */

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

/*==============================================================================
 * Includes
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>


/*==============================================================================
 * Public Types
 *============================================================================*/

/**
 * @brief GPIO port identifier type.
 */
typedef enum
{
    DRV_GPIO_PORT_A = 0,
    DRV_GPIO_PORT_B,
    DRV_GPIO_PORT_C,
    DRV_GPIO_PORT_D,
    DRV_GPIO_PORT_E,
    DRV_GPIO_PORT_F,
    DRV_GPIO_PORT_G,
    DRV_GPIO_PORT_H

} drv_gpio_port_t;

/**
 * @brief GPIO direction type.
 */
typedef enum
{
    DRV_GPIO_DIRECTION_INPUT = 0,
    DRV_GPIO_DIRECTION_OUTPUT

} drv_gpio_direction_t;

/**
 * @brief GPIO logic state type.
 */
typedef enum
{
    DRV_GPIO_STATE_LOW = 0,
    DRV_GPIO_STATE_HIGH

} drv_gpio_state_t;

/**
 * @brief GPIO interrupt edge type.
 */
typedef enum
{
    DRV_GPIO_IRQ_EDGE_RISING = 0,
    DRV_GPIO_IRQ_EDGE_FALLING,
    DRV_GPIO_IRQ_EDGE_BOTH

} drv_gpio_irq_edge_t;

/**
 * @brief GPIO interrupt callback function type.
 *
 * @param[in] port GPIO port identifier.
 * @param[in] pin  GPIO pin number.
 */
typedef void (*drv_gpio_irq_callback_t)(drv_gpio_port_t port, uint8_t pin);

/**
 * @brief GPIO configuration structure.
 */
typedef struct
{
	drv_gpio_port_t port;
    uint8_t pin;
    drv_gpio_direction_t direction;

} drv_gpio_config_t;


/*==============================================================================
 * Public Function Prototypes
 *============================================================================*/

/**
 * @brief Initializes a GPIO pin.
 *
 * @param[in] config Pointer to GPIO configuration.
 *
 * @pre config shall not be NULL.
 */
void DRV_GPIO_Init(const drv_gpio_config_t * const config);

/**
 * @brief Reads the logic state of a GPIO pin.
 *
 * @param[in] port GPIO port identifier.
 * @param[in] pin  GPIO pin number.
 *
 * @return Current GPIO state.
 */
drv_gpio_state_t DRV_GPIO_Read(drv_gpio_port_t port, uint8_t pin);

/**
 * @brief Writes a logic state to a GPIO pin.
 *
 * @param[in] port  GPIO port identifier.
 * @param[in] pin   GPIO pin number.
 * @param[in] state GPIO logic state.
 */
void DRV_GPIO_Write(drv_gpio_port_t port, uint8_t pin, drv_gpio_state_t state);

/**
 * @brief Toggles the logic state of a GPIO pin.
 *
 * @param[in] port GPIO port identifier.
 * @param[in] pin  GPIO pin number.
 */
void DRV_GPIO_Toggle(drv_gpio_port_t port, uint8_t pin);

/**
 * @brief Enables GPIO interrupt.
 *
 * @param[in] port GPIO port identifier.
 * @param[in] pin  GPIO pin number.
 * @param[in] edge Interrupt trigger edge.
 */
void DRV_GPIO_IRQ_Enable(drv_gpio_port_t port, uint8_t pin, drv_gpio_irq_edge_t edge, drv_gpio_irq_callback_t callback);

/**
 * @brief Disables GPIO interrupt.
 *
 * @param[in] port GPIO port identifier.
 * @param[in] pin  GPIO pin number.
 */
void DRV_GPIO_IRQ_Disable(drv_gpio_port_t port, uint8_t pin);

/**
 * @brief Clears GPIO interrupt flag.
 *
 * @param[in] port GPIO port identifier.
 * @param[in] pin  GPIO pin number.
 */
void DRV_GPIO_IRQ_ClearFlag(drv_gpio_port_t port, uint8_t pin);

#endif /* DRV_GPIO_H */
