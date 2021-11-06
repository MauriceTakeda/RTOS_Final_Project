/*
 * fuel_control.c
 *
 *  Created on: Sep 10, 2021
 *      Author: mauri
 */

#include <fuel_control.h>
#include <led_output.h>
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "os.h"

extern OS_FLAG_GRP Update_Flags;
extern fuel_control_t Fuel_Control;
extern OS_MUTEX Fuel_Mutex;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[FUEL_CONTROL_TASK_STACK_SIZE];

// Button Semaphore
static OS_SEM Btn_Semaphore;
static volatile bool state;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void fuel_control_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialize fuel control task
 ******************************************************************************/
void fuel_control_init(void)
{
  RTOS_ERR err;

  // Create Button Semaphore
  OSSemCreate(&Btn_Semaphore,
              "Button Semaphore",
              0,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Update Flag Group
  OSFlagCreate(&Update_Flags,
               "Update Flags",
               0,
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Mutex for Speed Setpoint data structure
  OSMutexCreate(&Fuel_Mutex,
                "Fuel Mutex",
                &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Fuel Control Task
  OSTaskCreate(&tcb,
               "fuel control task",
               fuel_control_task,
               DEF_NULL,
               FUEL_CONTROL_TASK_PRIO,
               &stack[0],
               (FUEL_CONTROL_TASK_STACK_SIZE / 10u),
               FUEL_CONTROL_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 *@brief
 *  Fuel control task
 ******************************************************************************/
static void fuel_control_task(void *arg)
{
    PP_UNUSED_PARAM(arg);

    RTOS_ERR err;
    OS_FLAGS event_flags;

    // Initialize hardware owned by task
    GPIO_DriveStrengthSet(BUTTON0_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(BUTTON0_port, BUTTON0_pin, gpioModeInput, BUTTON0_default);

    GPIO_DriveStrengthSet(BUTTON1_port, gpioDriveStrengthStrongAlternateStrong);
    GPIO_PinModeSet(BUTTON1_port, BUTTON1_pin, gpioModeInput, BUTTON1_default);

    GPIO_ExtIntConfig(BUTTON0_port, BUTTON0_pin, 6, true, true, true);
    GPIO_ExtIntConfig(BUTTON1_port, BUTTON1_pin, 7, true, true, true);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
    NVIC_SetPriority(GPIO_ODD_IRQn, 5);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    NVIC_SetPriority(GPIO_EVEN_IRQn, 6);

    // Infinite while()
    while (1)
    {
        // Pend on semaphore from ISRs
        OSSemPend(&Btn_Semaphore,
                  0,
                  OS_OPT_PEND_BLOCKING,
                  DEF_NULL,
                  &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        // Compute Required Values for the Fuel Control Data
        // Notify Physics task of any change in fuel using event flag
        OSMutexPend(&Fuel_Mutex,
                    0,
                    OS_OPT_PEND_BLOCKING,
                    DEF_NULL,
                    &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        CORE_DECLARE_IRQ_STATE;
        CORE_ENTER_CRITICAL();
        if(state == INCREMENT_FUEL) {
            Fuel_Control.fuel_burn_rate += DEFAULT_FUEL_INCREMENT;
            Fuel_Control.inc_count++;
        }
        else if(state == DECREMENT_FUEL) {
            Fuel_Control.fuel_burn_rate -= DEFAULT_FUEL_INCREMENT;
            Fuel_Control.dec_count++;
        }
        CORE_EXIT_CRITICAL();
        OSMutexPost(&Fuel_Mutex,
                    OS_OPT_POST_1,
                    &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        event_flags = OSFlagPost(&Update_Flags,
                                 EVENT_FLAG_UPDATE_FUEL,
                                 OS_OPT_POST_FLAG_SET,
                                 &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    }
}

/***************************************************************************//**
 * @brief
 *   Interrupt handler to service pressing/releasing of button 0
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  RTOS_ERR err;

  if(GPIO_PinInGet(BUTTON0_port,BUTTON0_pin) & GPIO_PinInGet(BUTTON1_port,BUTTON1_pin)) {
      state = INCREMENT_FUEL;
      OSSemPost(&Btn_Semaphore,
                OS_OPT_POST_ALL,
                &err);
      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  }
  uint32_t flags = GPIO_IntGetEnabled();
  GPIO_IntClear(flags);
}

/***************************************************************************//**
 * @brief
 *   Interrupt handler to service pressing/releasing of button 1
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  RTOS_ERR err;

  if(GPIO_PinInGet(BUTTON1_port,BUTTON1_pin) & GPIO_PinInGet(BUTTON0_port,BUTTON0_pin)) {
      state = DECREMENT_FUEL;
      OSSemPost(&Btn_Semaphore,
                OS_OPT_POST_ALL,
                &err);
      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  }
  uint32_t flags = GPIO_IntGetEnabled();
  GPIO_IntClear(flags);
}
