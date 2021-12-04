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
#include <math.h>
#include "os.h"

// TODO: Make Sure We Are Utilizing All Parameters From Physics Configuration Information
// TODO: Viable Timer Period
// TODO: Viable Configuration Values
// TODO: Make Sure Units Match Up When Performing Calculations
// TODO: Double Check Fuel Control, Angle, and Physics Task Before Moving On
// TODO: Set period for updates to 100ms for physics, capsense, and lcd. This will make it easy for LED timers as well. since os tick is 1ms just make percentage = to ms
// since it is out of 100ms

extern OS_FLAG_GRP Update_Flags;
extern OS_MUTEX Fuel_Mutex;
extern OS_MUTEX Angle_Mutex;
extern OS_MUTEX Flight_State_Mutex;
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
    .version               = PHYSICS_VERSION,
    .gravity               = PHYSICS_GRAVITY,
    .mass                  = PHYSICS_MASS_OF_VEHICLE,
    .graphing_limits       = { PHYSICS_GRAPHING_LIMITS_XMIN , PHYSICS_GRAPHING_LIMITS_XMAX},
    .option                = PHYSICS_OPTION,
    .max_thrust            = PHYSICS_MAX_THRUST,
    .init_fuel_mass        = PHYSICS_INIT_FUEL_MASS,
    .conversion_efficiency = PHYSICS_CONVERSION_EFFICIENCY,
    .max_landing_speed     = { PHYSICS_MAX_LANDING_SPEED_VERTICAL , PHYSICS_MAX_LANDING_SPEED_HORIZONTAL },
    .blackout              = { PHYSICS_BLACKOUT_ACCELERATION , PHYSICS_BLACKOUT_DURATION },
    .init_velocity         = { PHYSICS_INIT_VELOCITY_XVEL , PHYSICS_INIT_VELOCITY_YVEL },
    .init_horizontal_pos   = PHYSICS_INIT_HORIZONTAL_POSITION,
    .angle_of_attack       = PHYSICS_ANGLE_OF_ATTACK_CHANGE_QUANTA
};
static volatile uint64_t msTicks = 0;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void physics_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

void Physics_TimerCallback(void *p_tmr, void *p_arg) {
  RTOS_ERR err;
  OSSemPost(&Physics_Semaphore, OS_OPT_POST_ALL, &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

void SysTick_Handler(void)
{
  msTicks++;
//  if(msTicks >= 100) {
//      CAPSENSE_Sense();
//      read_capsense();
//      write_led();
//      msTicks = 0;
//  }
}

/***************************************************************************//**
 *@brief
 *  Initialize physics task
 ******************************************************************************/
void physics_init(void)
{
  RTOS_ERR err;

  // Create Semaphore for periodic task wakeup
  OSSemCreate(&Physics_Semaphore, "Physics Semaphore", 0, &err);
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
  OSMutexCreate(&Flight_State_Mutex, "Flight State Mutex", &err);
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
    int force_thrust;
    int acceleration;
    int x_acceleration;
    int y_acceleration;

    // Flight state initial values
    Flight_State.velocity.xvel = Physics.init_velocity.xvel;
    Flight_State.velocity.yvel = Physics.init_velocity.yvel;
    Flight_State.mass = Physics.mass + Physics.init_fuel_mass;
    Flight_State.horizontal_position = Physics.init_horizontal_pos;
    // TODO: Flight_State.vertical_position = ;
    Flight_State.vehicle_state = VEHICLE_STATE_HEALTHY;
    Flight_State.time = 0;
    Flight_State.blackout_time = 0;

    // Configure Systick Clock
    if(SysTick_Config(SystemCoreClockGet()/1000)) {
        EFM_ASSERT(true);
    }

    CPU_BOOLEAN started = OSTmrStart(&Physics_Timer,
               &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    // Infinite while()
    while (1)
    {
        set_events = 0;

        OSSemPend(&Physics_Semaphore, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
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

        OSMutexPend(&Flight_State_Mutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        // Calculate Time Since Last Update
        CORE_DECLARE_IRQ_STATE;
        CORE_ENTER_CRITICAL();
        Flight_State.time = msTicks;
        msTicks = 0;
        CORE_EXIT_CRITICAL();
        // Calculate Mass
        // TODO: Max Thrust Condition should prevent fuel burn rate from increasing
        // TODO: Save Acceleration and Thrust
        if(Flight_State.mass <= Physics.mass) { // If no fuel left
            Flight_State.mass = Physics.mass;
            fuel_burn_rate = 0;
        }
        else { // If there is fuel to burn
            Flight_State.mass = Flight_State.mass - fuel_burn_rate * Flight_State.time; // mass_curr = mass_prev - fuel_burn_rate * time
        }
        // Calculate X and Y Components of Velocity
        force_thrust = Physics.conversion_efficiency * fuel_burn_rate * Flight_State.time; // F_Thrust = Conversion Efficiency * Change in Mass
        if(force_thrust > Physics.max_thrust) {
            force_thrust = Physics.max_thrust;
        }
        x_acceleration = (force_thrust * cos(angle_of_attack)) / Flight_State.mass; // a_x = F_thrust_x / mass
        y_acceleration = (force_thrust * sin(angle_of_attack) - Flight_State.mass * Physics.gravity) / Flight_State.mass; // a_y = (F_thrust_y - F_gravity) / mass
        Flight_State.velocity.xvel += x_acceleration * Flight_State.time; // v_x_f = v_x_i + a_x * t
        Flight_State.velocity.yvel += y_acceleration * Flight_State.time; // v_y_f = v_y_i + a_y * t
        // Calculate X and Y Position
        Flight_State.horizontal_position += Flight_State.velocity.xvel * Flight_State.time; // x_f = x_i + v_x * t
        Flight_State.vertical_position += Flight_State.velocity.yvel * Flight_State.time; // y_f = y_i + v_y * t
        // TODO: Determine Vehicle State based on blackout time and/or positioning (crash)
        // If Horizontal Position <= Min || Horizontal Position >= Max -> Crashed
        // If Vertical Position == Land && Velocity >= Max_Landing_Speed -> Crashed
        // If Blackout then Blackout_Time += msTicks
        // If Acceleration >= Blackout_Acceleration -> Blackout (Burn_Rate == 0) Need to prevent burn rate from changing and zero it
        // If Blackout_Time >= Blackout_Duration -> Blackout_Time = 0 and Allow Fuel Burn Rate to Change Again and Change to Healthy Flight
        // Else Healthy Flight
        OSMutexPost(&Flight_State_Mutex, OS_OPT_POST_1, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        // TODO: Post to Graphics and LED Task

    }
}

/*
 * Know ------------
 *
 * Acceleration
 * Angle
 *
 * Calculate -------
 *
 * Mass
 * Position
 * Time
 * Velocity
 * State of Vehicle
 */




