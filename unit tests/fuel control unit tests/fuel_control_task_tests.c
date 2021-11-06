#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"
#include "fuel_control_task.h"
#include <time.h>

CTEST_DATA(fuelcontroltask) {
    int fuel_burn_rate;
    int btn_pressed[1];
    int btn_press_count;
};

CTEST_SETUP(fuelcontroltask) {
    data->btn_pressed[0] = 1;
    data->btn_press_count = sizeof(data->btn_pressed) / sizeof(data->btn_pressed[0]);
    data->fuel_burn_rate = 0;
    fuel_control_task(&(data->fuel_burn_rate), data->btn_pressed, data->btn_press_count);
}

CTEST2(fuelcontroltask, test_one_press) {
    ASSERT_EQUAL(1, data->fuel_burn_rate);
}

CTEST_DATA(fuelcontroltask1) {
    int fuel_burn_rate;
    int btn_pressed[4];
    int btn_press_count;
};

CTEST_SETUP(fuelcontroltask1) {
    data->btn_pressed[0] = 1;
    data->btn_pressed[1] = 1;
    data->btn_pressed[2] = 1;
    data->btn_pressed[3] = 0;
    data->btn_press_count = sizeof(data->btn_pressed) / sizeof(data->btn_pressed[0]);
    data->fuel_burn_rate = 0;
    fuel_control_task(&(data->fuel_burn_rate), data->btn_pressed, data->btn_press_count);
}

CTEST2(fuelcontroltask1, test_mult_press_pos) {
    ASSERT_EQUAL(2, data->fuel_burn_rate);
}

CTEST_DATA(fuelcontroltask2) {
    int fuel_burn_rate;
    int btn_pressed[5];
    int btn_press_count;
};

CTEST_SETUP(fuelcontroltask2) {
    data->btn_pressed[0] = 0;
    data->btn_pressed[1] = 0;
    data->btn_pressed[2] = 1;
    data->btn_pressed[3] = 0;
    data->btn_pressed[4] = 0;
    data->btn_press_count = sizeof(data->btn_pressed) / sizeof(data->btn_pressed[0]);
    data->fuel_burn_rate = 0;
    fuel_control_task(&(data->fuel_burn_rate), data->btn_pressed, data->btn_press_count);
}

CTEST2(fuelcontroltask2, test_mult_press_neg) {
    ASSERT_EQUAL(-3, data->fuel_burn_rate);
}