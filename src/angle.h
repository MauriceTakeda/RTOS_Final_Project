/*
 * angle.h
 *
 *  Created on: Sep 10, 2021
 *      Author: mauri
 */

#ifndef SRC_HEADER_FILES_ANGLE_H_
#define SRC_HEADER_FILES_ANGLE_H_

#include "gpio.h"

#define ANGLE_TASK_STACK_SIZE      512

#define ANGLE_TASK_PRIO            19

#define ANGLE_TIMER_PERIOD         10 // angle timer period in OS TICKS
#define DEFAULT_ANGLE_INCREMENT     1

enum position {
  none = -1,
  farLeft,
  left,
  right,
  farRight,
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the vehicle direction data
//----------------------------------------------------------------------------------------------------------------------------------
typedef struct angle_s {

    // current angle
    int angle;

    int inc_count;
    int dec_count;

} angle_t;

void angle_init(void);

#endif /* SRC_HEADER_FILES_ANGLE_H_ */
