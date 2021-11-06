#include "fuel_control_task.h"
#include <stdio.h>

void fuel_control_task(int * fuel_burn_rate, int * btn_pressed, int btn_press_count) {
    for(int i = 0; i < btn_press_count; i++) {
        if(btn_pressed[i] == 0) {
            (*fuel_burn_rate)--;
        }
        else if(btn_pressed[i] == 1) {
            (*fuel_burn_rate)++;
        }
    }
}