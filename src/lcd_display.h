/*
 * lcd_display.h
 *
 *  Created on: Oct 25, 2021
 *      Author: mauri
 */

#ifndef SRC_HEADER_FILES_LCD_DISPLAY_H_
#define SRC_HEADER_FILES_LCD_DISPLAY_H_

#include "gpio.h"

#define LCD_DISPLAY_TASK_STACK_SIZE      512

#define LCD_DISPLAY_TASK_PRIO            17

#define LCD_DISPLAY_TIMER_PERIOD         30

void lcd_display_init(void);

#endif /* SRC_HEADER_FILES_LCD_DISPLAY_H_ */
