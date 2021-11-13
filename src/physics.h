/*
 * physics.h
 *
 *  Created on: Oct 24, 2021
 *      Author: mauri
 */

#ifndef SRC_HEADER_FILES_PHYSICS_H_
#define SRC_HEADER_FILES_PHYSICS_H_

#include "gpio.h"

#define PHYSICS_TASK_STACK_SIZE                  512

#define PHYSICS_TASK_PRIO                        18

#define PHYSICS_TIMER_PERIOD                     3

enum options {
  OPTION_A, // Capsense slider adjusts angle of attack, buttons control fuel burn rate (+/-)
  OPTION_B, // Capsense slider adjusts angle of attack, buttons burst fuel while held (one button: nominal thrust, the other: MAX thrust)
  OPTION_C, // Capsense slider adjusts fuel burn rate, buttons adjust angle of attack in increments when tapped or held.
  OPTION_D, // Capsense slider adjusts fuel burn rate, buttons burst a quanta of rotational rate to the angle of attack in increments when tapped or held.
  OPTION_E, // Etc—come up with your own ideas, and get agreement from instructor.
};

#define PHYSICS_VERSION                          1
#define PHYSICS_DEF_GRAVITY                      9
#define PHYSICS_DEF_MASS_OF_VEHICLE              5
#define PHYSICS_DEF_GRAPHING_LIMITS_XMIN         10
#define PHYSICS_DEF_GRAPHING_LIMITS_XMAX         10
#define PHYSICS_DEF_OPTION                       OPTION_A
#define PHYSICS_DEF_INIT_FUEL_MASS               5
#define PHYSICS_DEF_MAX_THRUST                   5
#define PHYSICS_DEF_MAX_LANDING_SPEED_VERTICAL   2
#define PHYSICS_DEF_MAX_LANDING_SPEED_HORIZONTAL 2
#define PHYSICS_DEF_BLACKOUT_ACCELERATION        5
#define PHYSICS_DEF_BLACKOUT_DURATION            3
#define PHYSICS_DEF_INIT_VELOCITY_XVEL           1
#define PHYSICS_DEF_INIT_VELOCITY_YVEL           1
#define PHYSICS_DEF_INIT_HORIZONTAL_POSITION     5

typedef struct graph_lim_s {
  int xmin; // (cm)
  int xmax; // (cm)
} graph_lim_t;

typedef struct max_landing_speed_s {
  uint32_t vertical; // (cm/s)
  uint32_t horizontal; // (mm/s)
} max_landing_speed_t;

typedef struct blackout_s {
  uint32_t acceleration; // (mm/s^2)
  uint32_t duration; // (ms)
} blackout_t;

typedef struct velocity_s {
  int xvel; // (cm/s)
  int yvel; // (cm/s)
} velocity_t;

typedef struct angle_of_atk_s {
  int angle_change_quanta; // (mrad)
  int angle_change_rate;   // (mrad/s)
} angle_of_atk_t;

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the physics configuration data
//----------------------------------------------------------------------------------------------------------------------------------
typedef struct physics_s {
  uint32_t version;
  uint32_t gravity; // (mm/s^2)
  uint32_t mass;    // (kg)
  graph_lim_t graphing_limits;
  uint32_t option;
  uint32_t max_thrust;  // (N)
  uint32_t init_fuel_mass;  // (kg)
  uint32_t conversion_efficiency;  // (%)
  uint32_t fuel_energy_density;  // kJ/g
  max_landing_speed_t max_landing_speed;
  blackout_t blackout;
  velocity_t init_velocity;
  int init_horizontal_pos; // (mm)
  angle_of_atk_t angle_of_attack;
} physics_t;

enum vehicle_state {
  VEHICLE_STATE_HEALTHY,
  VEHICLE_STATE_BLACKOUT,
  VEHICLE_STATE_CRASHED,
};

// TODO: Create Systick Handler with private variable for msTicks and use to calculate start and end times
// TODO: Init some of these values in beginning of physics task before while loop
typedef struct flight_state_s {
  velocity_t velocity;
  uint32_t mass; // (kg)
  uint32_t horizontal_position; // (mm)
  uint32_t vertical_position; // (mm)
  uint32_t vehicle_state;
  uint32_t start_time; // (ms)
  uint32_t end_time; // (ms)
  uint32_t blackout_start_time; // (ms)
  uint32_t blackout_end_time; // (ms)
} flight_state_t;

void physics_init(void);

#endif /* SRC_HEADER_FILES_PHYSICS_H_ */
