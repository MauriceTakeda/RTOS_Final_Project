/*
 * angle.c
 *
 *  Created on: Sep 10, 2021
 *      Author: mauri
 */
#include <angle.h>
#include <fuel_control.h>
#include "os.h"
#include "capsense.h"

extern OS_MUTEX Angle_Mutex;
extern angle_t Angle;
extern OS_FLAG_GRP Update_Flags;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[ANGLE_TASK_STACK_SIZE];
static OS_TMR Angle_Timer;
static OS_SEM Angle_Semaphore;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void angle_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

void Angle_TimerCallback(void *p_tmr, void *p_arg) {
  RTOS_ERR err;
  OSSemPost(&Angle_Semaphore,
            OS_OPT_POST_ALL,
            &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 *@brief
 *  Initialize angle task
 ******************************************************************************/
void angle_init(void)
{
  RTOS_ERR err;

  // Create Semaphore for periodic task wakeup
  OSSemCreate(&Angle_Semaphore,
              "Angle Semaphore",
              0,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Timer for periodic task wakeup
  OSTmrCreate(&Angle_Timer,
              "Angle Timer",
              2,
              ANGLE_TIMER_PERIOD,
              OS_OPT_TMR_PERIODIC,
              &Angle_TimerCallback,
              DEF_NULL,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Mutex for Angle data structure
  OSMutexCreate(&Angle_Mutex,
                "Angle Mutex",
                &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Angle Task
  OSTaskCreate(&tcb,
               "angle task",
               angle_task,
               DEF_NULL,
               ANGLE_TASK_PRIO,
               &stack[0],
               (ANGLE_TASK_STACK_SIZE / 10u),
               ANGLE_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 *@brief
 *  Angle task
 ******************************************************************************/
static void angle_task(void *arg)
{
    PP_UNUSED_PARAM(arg);

    RTOS_ERR err;
    OS_FLAGS event_flags;
    bool post_flag;

    // Initialize hardware owned by task
    GPIO_DriveStrengthSet(CSEN0_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(CSEN0_port, CSEN0_pin, gpioModeInput, CSEN0_default);

    GPIO_DriveStrengthSet(CSEN1_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(CSEN1_port, CSEN1_pin, gpioModeInput, CSEN1_default);

    GPIO_DriveStrengthSet(CSEN2_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(CSEN2_port, CSEN2_pin, gpioModeInput, CSEN2_default);

    GPIO_DriveStrengthSet(CSEN3_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(CSEN3_port, CSEN3_pin, gpioModeInput, CSEN3_default);

    CAPSENSE_Init();

    CPU_BOOLEAN started = OSTmrStart(&Angle_Timer,
               &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    // Infinite while()
    while (1)
    {
        post_flag = false;
        OSSemPend(&Angle_Semaphore,
                  0,
                  OS_OPT_PEND_BLOCKING,
                  DEF_NULL,
                  &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        OSMutexPend(&Angle_Mutex,
                    0,
                    OS_OPT_PEND_BLOCKING,
                    DEF_NULL,
                    &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        CAPSENSE_Sense();
        if((CAPSENSE_getPressed(farLeft) || CAPSENSE_getPressed(left)) && ~(CAPSENSE_getPressed(farRight) || CAPSENSE_getPressed(right))) {
            Angle.angle += DEFAULT_ANGLE_INCREMENT;
            Angle.inc_count++;
            post_flag = true;
        }
        else if((CAPSENSE_getPressed(farRight) || CAPSENSE_getPressed(right)) && ~(CAPSENSE_getPressed(farLeft) || CAPSENSE_getPressed(left))) {
            Angle.angle -= DEFAULT_ANGLE_INCREMENT;
            Angle.dec_count++;
            post_flag = true;
        }
        OSMutexPost(&Angle_Mutex,
                    OS_OPT_POST_1,
                    &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        if(post_flag) {
            event_flags = OSFlagPost(&Update_Flags,
                                     EVENT_FLAG_UPDATE_ANGLE,
                                     OS_OPT_POST_FLAG_SET,
                                     &err);
            EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        }
    }
}




