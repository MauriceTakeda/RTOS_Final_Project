/*
 * fuel_control.h
 *
 *  Created on: Sep 10, 2021
 *      Author: mauri
 */

#ifndef SRC_HEADER_FILES_FUEL_CONTROL_H_
#define SRC_HEADER_FILES_FUEL_CONTROL_H_

#include "gpio.h"
#include <stdbool.h>

#define FUEL_CONTROL_TASK_STACK_SIZE      512

#define FUEL_CONTROL_TASK_PRIO            20

#define INCREMENT_FUEL                     true
#define DECREMENT_FUEL                     false
#define DEFAULT_FUEL_INCREMENT 1

enum update_flag {
  EVENT_FLAG_UPDATE_FUEL = (1u << 0),
  EVENT_FLAG_UPDATE_ANGLE = (1u << 1),
  EVENT_FLAG_UPDATE_ALL,
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the fuel control data
//----------------------------------------------------------------------------------------------------------------------------------
typedef struct fuel_control_s {

    // current fuel burn rate
    int fuel_burn_rate;

    // Count of fuel increments
    int inc_count;
    // Count of fuel decrements
    int dec_count;
} fuel_control_t;

/***************************************************************************//**
 * Initialize fuel control example
 ******************************************************************************/
void fuel_control_init(void);

#endif /* SRC_HEADER_FILES_FUEL_CONTROL_H_ */
