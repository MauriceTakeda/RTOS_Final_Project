/*
 * led_output.h
 *
 *  Created on: Sep 10, 2021
 *      Author: mauri
 */

#ifndef HEADER_FILES_LED_OUTPUT_H_
#define HEADER_FILES_LED_OUTPUT_H_

#include "gpio.h"

#define LED_OUTPUT_TASK_STACK_SIZE      512

#define LED_OUTPUT_TASK_PRIO            18

void led_output_init(void);

#endif /* HEADER_FILES_LED_OUTPUT_H_ */
