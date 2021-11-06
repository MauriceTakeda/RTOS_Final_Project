
#ifndef __LED_TASK__
#define __LED_TASK__

#define MAX_THRUST 5
#define BLACKOUT_ACCELERATION 5
#define DEFAULT_FREQUENCY 2

enum flight {
    healthy,
    blackout,
    crashed,
};

struct led_input_t {
    int thrust;
    int acceleration;
    int flight_state;
};

struct led_output_t {
    float duty_cycle;
    int frequency;
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Run the led task
///
/// @param[in] led_in
/// @param[in] led0_out
/// @param[in] led1_out
//----------------------------------------------------------------------------------------------------------------------------------
void led_task(struct led_input_t led_in, struct led_output_t *led0_out, struct led_output_t *led1_out);

#endif // __LED_TASK__