/*
 * led.c
 *
 *  Created on: Sep 10, 2021
 *      Author: mauri
 */

#include <led_output.h>
#include <physics.h>
//#include "sl_simple_led.h"
//#include "sl_simple_led_instances.h"
#include "os.h"

extern OS_FLAG_GRP Alert_Event_Flags;
extern flight_state_t Flight_State;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[LED_OUTPUT_TASK_STACK_SIZE];
static OS_TMR LED1_ON_Timer, LED1_OFF_Timer, LED0_ON_Timer, LED0_OFF_Timer;
static float led0_dc, led1_dc;
/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void led_output_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

void LED0_ON_TimerCallback(void *p_tmr, void *p_arg) {
  RTOS_ERR err;
  GPIO_PinOutSet(LED0_port, LED0_pin);
  OSTmrStart(&LED0_OFF_Timer, &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

void LED0_OFF_TimerCallback(void *p_tmr, void *p_arg) {
  GPIO_PinOutClear(LED0_port, LED0_pin);
}

void LED1_OFF_TimerCallback(void *p_tmr, void *p_arg) {
  GPIO_PinOutClear(LED1_port, LED1_pin);
}

void LED1_ON_TimerCallback(void *p_tmr, void *p_arg) {
  RTOS_ERR err;
  if(0) { // TODO: healthy
      GPIO_PinOutSet(LED1_port, LED1_pin);
      OSTmrSet(&LED1_OFF_Timer, 1, 0, &LED1_OFF_TimerCallback, DEF_NULL, &err);
      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
      OSTmrStart(&LED1_OFF_Timer, &err);
      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  }
  else { // TODO: crashed or blackout
      GPIO_PinOutToggle(LED1_port, LED1_pin);
  }
}

/***************************************************************************//**
 *@brief
 *  Initialize LED output task
 ******************************************************************************/
void led_output_init(void)
{
  RTOS_ERR err;

  // Create Timer for periodic task wakeup
  OSTmrCreate(&LED1_ON_Timer,
              "LED1 ON Timer",
              0,
              10,
              OS_OPT_TMR_PERIODIC,
              &LED1_ON_TimerCallback,
              DEF_NULL,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTmrCreate(&LED1_OFF_Timer,
              "LED1 OFF Timer",
              9,
              0,
              OS_OPT_TMR_ONE_SHOT,
              &LED1_OFF_TimerCallback,
              DEF_NULL,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTmrCreate(&LED0_ON_Timer,
              "LED0 ON Timer",
              0,
              20,
              OS_OPT_TMR_PERIODIC,
              &LED0_ON_TimerCallback,
              DEF_NULL,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTmrCreate(&LED0_OFF_Timer,
              "LED0 OFF Timer",
              10,
              0,
              OS_OPT_TMR_ONE_SHOT,
              &LED0_OFF_TimerCallback,
              DEF_NULL,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create LED Output Task
  OSTaskCreate(&tcb,
               "led output task",
               led_output_task,
               DEF_NULL,
               LED_OUTPUT_TASK_PRIO,
               &stack[0],
               (LED_OUTPUT_TASK_STACK_SIZE / 10u),
               LED_OUTPUT_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 *@brief
 *  LED output task
 ******************************************************************************/
static void led_output_task(void *arg)
{
    PP_UNUSED_PARAM(arg);

    RTOS_ERR err;
    OS_FLAGS event_flags;

    // Initialize hardware owned by task
    // Set LED ports to be standard output drive with default off (cleared)
    GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

    GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

    OSTmrStart(&LED0_ON_Timer, &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    OSTmrStart(&LED1_ON_Timer, &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    // Infinite while()
    while (1)
    {
        //TODO: Pend on Flag?

        //TODO: Get Flight_State Mutex (Vehicle State, Thrust, Acceleration)

        // Update LED0 PWM if Thrust Changes (event flag)
        led0_dc = Flight_State.thrust / (float)PHYSICS_MAX_THRUST;

        // Update LED1 Duty Cycle and Frequency if acceleration or vehicle state changes (event flag)
        switch(Flight_State.vehicle_state) {
          case VEHICLE_STATE_HEALTHY:
            led1_dc = Flight_State.acceleration / (float)PHYSICS_BLACKOUT_ACCELERATION;
            break;
          case VEHICLE_STATE_BLACKOUT:
            OSTmrSet(&LED1_ON_Timer, 0, 3, &LED1_ON_TimerCallback, DEF_NULL, &err);
            EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
            break;
          case VEHICLE_STATE_CRASHED:
            OSTmrSet(&LED1_ON_Timer, 0, 10, &LED1_ON_TimerCallback, DEF_NULL, &err);
            EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
            break;
          default:
            EFM_ASSERT(true);
            break;
        }

        //TODO: Release Flight_State Mutex
    }
}
