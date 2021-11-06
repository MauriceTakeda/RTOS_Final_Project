#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"
#include "angle_task.h"
#include <time.h>

CTEST_DATA(angletask) {
    int angle;
    struct capsense_t capsense[1];
    int size;
};

CTEST_SETUP(angletask) {
    data->capsense[0].slider_pos = left;
    data->capsense[0].time = 3;
    data->size = sizeof(data->capsense) / sizeof(data->capsense[0]);
    data->angle = 0;
    angle_task(&(data->angle), data->capsense, data->size);
}

CTEST2(angletask, test_left) {
    ASSERT_EQUAL(-3, data->angle);
}

CTEST_DATA(angletask1) {
    int angle;
    struct capsense_t capsense[2];
    int size;
};

CTEST_SETUP(angletask1) {
    data->capsense[0].slider_pos = left;
    data->capsense[0].time = 3;
    data->capsense[1].slider_pos = farRight;
    data->capsense[1].time = 5;
    data->size = sizeof(data->capsense) / sizeof(data->capsense[0]);
    data->angle = 0;
    angle_task(&(data->angle), data->capsense, data->size);
}

CTEST2(angletask1, test_farRight) {
    ASSERT_EQUAL(2, data->angle);
}

CTEST_DATA(angletask2) {
    int angle;
    struct capsense_t capsense[3];
    int size;
};

CTEST_SETUP(angletask2) {
    data->capsense[0].slider_pos = left;
    data->capsense[0].time = 3;
    data->capsense[1].slider_pos = farRight;
    data->capsense[1].time = 3;
    data->capsense[2].slider_pos = right;
    data->capsense[2].time = 1;
    data->size = sizeof(data->capsense) / sizeof(data->capsense[0]);
    data->angle = 0;
    angle_task(&(data->angle), data->capsense, data->size);
}

CTEST2(angletask2, test_right) {
    ASSERT_EQUAL(1, data->angle);
}

CTEST_DATA(angletask3) {
    int angle;
    struct capsense_t capsense[4];
    int size;
};

CTEST_SETUP(angletask3) {
    data->capsense[0].slider_pos = left;
    data->capsense[0].time = 3;
    data->capsense[1].slider_pos = farRight;
    data->capsense[1].time = 3;
    data->capsense[2].slider_pos = right;
    data->capsense[2].time = 1;
    data->capsense[3].slider_pos = farLeft;
    data->capsense[3].time = 3;
    data->size = sizeof(data->capsense) / sizeof(data->capsense[0]);
    data->angle = 0;
    angle_task(&(data->angle), data->capsense, data->size);
}

CTEST2(angletask3, test_farLeft) {
    ASSERT_EQUAL(-2, data->angle);
}

CTEST_DATA(angletask4) {
    int angle;
    struct capsense_t capsense[2];
    int size;
};

CTEST_SETUP(angletask4) {
    data->capsense[0].slider_pos = left;
    data->capsense[0].time = 3;
    data->capsense[1].slider_pos = none;
    data->capsense[1].time = 3;
    data->size = sizeof(data->capsense) / sizeof(data->capsense[0]);
    data->angle = 0;
    angle_task(&(data->angle), data->capsense, data->size);
}

CTEST2(angletask4, test_none) {
    ASSERT_EQUAL(-3, data->angle);
}