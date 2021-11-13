/*
 * physics.c
 *
 *  Created on: Oct 24, 2021
 *      Author: mauri
 */
#include <angle.h>
#include <fuel_control.h>
#include <physics.h>
#include <stdlib.h>
#include "os.h"

// TODO: Viable Timer Period
// TODO: Viable Configuration Values

extern OS_FLAG_GRP Update_Flags;
extern OS_MUTEX Fuel_Mutex;
extern OS_MUTEX Angle_Mutex;
extern fuel_control_t Fuel_Control;
extern angle_t Angle;
extern flight_state_t Flight_State;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[PHYSICS_TASK_STACK_SIZE];
static OS_TMR Physics_Timer;
static OS_SEM Physics_Semaphore;
static physics_t Physics = {
    .version             = PHYSICS_VERSION,
    .gravity             = PHYSICS_DEF_GRAVITY,
    .mass                = PHYSICS_DEF_MASS_OF_VEHICLE,
    .graphing_limits     = { PHYSICS_DEF_GRAPHING_LIMITS_XMIN , PHYSICS_DEF_GRAPHING_LIMITS_XMAX},
    .option              = PHYSICS_DEF_OPTION,
    .max_thrust          = PHYSICS_DEF_MAX_THRUST,
    .init_fuel_mass      = PHYSICS_DEF_INIT_FUEL_MASS,
    // TODO: .conversion_efficiency
    // TODO: .fuel_energy_density
    .max_landing_speed   = { PHYSICS_DEF_MAX_LANDING_SPEED_VERTICAL , PHYSICS_DEF_MAX_LANDING_SPEED_HORIZONTAL },
    .blackout            = { PHYSICS_DEF_BLACKOUT_ACCELERATION , PHYSICS_DEF_BLACKOUT_DURATION },
    .init_velocity       = { PHYSICS_DEF_INIT_VELOCITY_XVEL , PHYSICS_DEF_INIT_VELOCITY_YVEL },
    .init_horizontal_pos = PHYSICS_DEF_INIT_HORIZONTAL_POSITION
    // TODO: .angle_of_atk
};

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void physics_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

void Physics_TimerCallback(void *p_tmr, void *p_arg) {
  RTOS_ERR err;
  OSSemPost(&Physics_Semaphore,
            OS_OPT_POST_ALL,
            &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 *@brief
 *  Initialize physics task
 ******************************************************************************/
void physics_init(void)
{
  RTOS_ERR err;

  // Create Semaphore for periodic task wakeup
  OSSemCreate(&Physics_Semaphore,
              "Physics Semaphore",
              0,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Timer for periodic task wakeup
  OSTmrCreate(&Physics_Timer,
              "Physics Timer",
              2,
              PHYSICS_TIMER_PERIOD,
              OS_OPT_TMR_PERIODIC,
              &Physics_TimerCallback,
              DEF_NULL,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Mutex for Physics data structure
  OSMutexCreate(&Physics_Mutex,
                "Physics Mutex",
                &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Physics Task
  OSTaskCreate(&tcb,
               "physics task",
               physics_task,
               DEF_NULL,
               PHYSICS_TASK_PRIO,
               &stack[0],
               (PHYSICS_TASK_STACK_SIZE / 10u),
               PHYSICS_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 *@brief
 *  Physics task
 ******************************************************************************/
static void physics_task(void *arg)
{
    PP_UNUSED_PARAM(arg);

    RTOS_ERR err;
    OS_FLAGS event_flags;
    int set_events;
    OS_FLAGS flag_set;

    int fuel_burn_rate;
    int angle_of_attack;

    // Infinite while()
    while (1)
    {
        set_events = 0;

        OSSemPend(&Physics_Semaphore,
                  0,
                  OS_OPT_PEND_BLOCKING,
                  DEF_NULL,
                  &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        event_flags = OSFlagPend(&Update_Flags,
                                 EVENT_FLAG_UPDATE_ALL,
                                 0,
                                 OS_OPT_PEND_FLAG_SET_ANY |
                                 OS_OPT_PEND_NON_BLOCKING |
                                 OS_OPT_PEND_FLAG_CONSUME,
                                 DEF_NULL,
                                 &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        if(EVENT_FLAG_UPDATE_FUEL & event_flags) {
            OSMutexPend(&Fuel_Mutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
            EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
            fuel_burn_rate = Fuel_Control.fuel_burn_rate;
            OSMutexPost(&Fuel_Mutex, OS_OPT_POST_1, &err);
            EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        }

        if(EVENT_FLAG_UPDATE_ANGLE & event_flags) {
            OSMutexPend(&Angle_Mutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
            EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
            angle_of_attack = Angle.angle;
            OSMutexPost(&Angle_Mutex, OS_OPT_POST_1, &err);
            EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        }

        OSMutexPend(&Physics_Mutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        // TODO: Perform Physics Calculations & Update Flight_State Structure
        OSMutexPost(&Angle_Mutex, OS_OPT_POST_1, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        // TODO: Post to Graphics and LED Task

    }
}




