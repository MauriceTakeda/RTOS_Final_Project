/*
 * lcd_display.c
 *
 *  Created on: Oct 25, 2021
 *      Author: mauri
 */
#include <angle.h>
#include <fuel_control.h>
#include <lcd_display.h>
#include "glib.h"
#include "dmd.h"
#include "os.h"

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[LCD_DISPLAY_TASK_STACK_SIZE];
static OS_SEM LCD_Display_Semaphore;
static GLIB_Context_t glibContext;
static int currentLine = 0;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void lcd_display_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 *@brief
 *  Initialize lcd display task
 ******************************************************************************/
void lcd_display_init(void)
{
  RTOS_ERR err;
  uint32_t status;

  // Enable the memory lcd
  status = sl_board_enable_display();
  EFM_ASSERT(status == SL_STATUS_OK);

  // Initialize the DMD support for memory lcd display
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  // Initialize the glib context
  status = GLIB_contextInit(&glibContext);
  EFM_ASSERT(status == GLIB_OK);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  // Fill lcd with background color
  GLIB_clear(&glibContext);

  // Use Narrow font
  GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNarrow6x8);

  // Create Semaphore for periodic task wakeup
  OSSemCreate(&LCD_Display_Semaphore,
              "LCD Display Semaphore",
              0,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create LCD Display Task
  OSTaskCreate(&tcb,
               "lcd display task",
               lcd_display_task,
               DEF_NULL,
               LCD_DISPLAY_TASK_PRIO,
               &stack[0],
               (LCD_DISPLAY_TASK_STACK_SIZE / 10u),
               LCD_DISPLAY_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 *@brief
 *  LCD Display task
 ******************************************************************************/
static void lcd_display_task(void *arg)
{
    PP_UNUSED_PARAM(arg);

    RTOS_ERR err;

    int32_t PolyPoints[6] = {60, 12, 63, 0, 66, 12};

    // TODO: Initialize hardware owned by task

    // Infinite while()
    while (1)
    {
//        OSSemPend(&LCD_Display_Semaphore,
//                  0,
//                  OS_OPT_PEND_BLOCKING,
//                  DEF_NULL,
//                  &err);
//        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        OSTimeDly(100, OS_OPT_TIME_DLY, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        GLIB_clear(&glibContext);
        GLIB_drawPolygonFilled(&glibContext, 3, PolyPoints);
        if(PolyPoints[1] < 127 && PolyPoints[3] < 127 && PolyPoints[5] < 127) {
            PolyPoints[1]++;
            PolyPoints[3]++;
            PolyPoints[5]++;
        }
        else {
            PolyPoints[1] = 10;
            PolyPoints[3] = 0;
            PolyPoints[5] = 10;
        }
        DMD_updateDisplay();

    }
}






