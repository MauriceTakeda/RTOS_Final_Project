#include "led_task.h"
#include <stdio.h>
#include <stdlib.h>

void led_task(struct led_input_t led_in, struct led_output_t *led0_out, struct led_output_t *led1_out) {
    (*led0_out).duty_cycle = abs(led_in.thrust) / (float)MAX_THRUST;
    (*led0_out).frequency = DEFAULT_FREQUENCY;

    switch (led_in.flight_state) {
        case healthy:
            (*led1_out).duty_cycle = abs(led_in.acceleration) / (float)BLACKOUT_ACCELERATION;
            (*led1_out).frequency = DEFAULT_FREQUENCY;
            break;
        case blackout:
            (*led1_out).duty_cycle = 0.5;
            (*led1_out).frequency = 3;
            break;
        case crashed:
            (*led1_out).duty_cycle = 0.5;
            (*led1_out).frequency = 1;
            break;
        default:
            break;
    }
}