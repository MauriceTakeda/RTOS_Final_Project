#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"
#include "led_task.h"
#include <time.h>

CTEST_DATA(ledtask) {
    struct led_input_t led_in;
    struct led_output_t led0_out;
    struct led_output_t led1_out;
};

CTEST_SETUP(ledtask) {
    data->led_in.acceleration = 1;
    data->led_in.flight_state = healthy;
    data->led_in.thrust = 1;
    led_task(data->led_in, &(data->led0_out), &(data->led1_out));
}

CTEST2(ledtask, test_flight_healthy) {
    ASSERT_EQUAL(1 / (float)MAX_THRUST, data->led0_out.duty_cycle);
    ASSERT_EQUAL(DEFAULT_FREQUENCY, data->led0_out.frequency);
    ASSERT_EQUAL(1 / (float)BLACKOUT_ACCELERATION, data->led1_out.duty_cycle);
    ASSERT_EQUAL(DEFAULT_FREQUENCY, data->led1_out.frequency);
}

CTEST_DATA(ledtask1) {
    struct led_input_t led_in;
    struct led_output_t led0_out;
    struct led_output_t led1_out;
};

CTEST_SETUP(ledtask1) {
    data->led_in.acceleration = BLACKOUT_ACCELERATION;
    data->led_in.flight_state = blackout;
    data->led_in.thrust = 2;
    led_task(data->led_in, &(data->led0_out), &(data->led1_out));
}

CTEST2(ledtask1, test_flight_blackout) {
    ASSERT_EQUAL(2 / (float)MAX_THRUST, data->led0_out.duty_cycle);
    ASSERT_EQUAL(DEFAULT_FREQUENCY, data->led0_out.frequency);
    ASSERT_EQUAL(0.5, data->led1_out.duty_cycle);
    ASSERT_EQUAL(3, data->led1_out.frequency);
}

CTEST_DATA(ledtask2) {
    struct led_input_t led_in;
    struct led_output_t led0_out;
    struct led_output_t led1_out;
};

CTEST_SETUP(ledtask2) {
    data->led_in.acceleration = 0;
    data->led_in.flight_state = crashed;
    data->led_in.thrust = 0;
    led_task(data->led_in, &(data->led0_out), &(data->led1_out));
}

CTEST2(ledtask2, test_flight_blackout) {
    ASSERT_EQUAL(0, data->led0_out.duty_cycle);
    ASSERT_EQUAL(DEFAULT_FREQUENCY, data->led0_out.frequency);
    ASSERT_EQUAL(0.5, data->led1_out.duty_cycle);
    ASSERT_EQUAL(1, data->led1_out.frequency);
}

