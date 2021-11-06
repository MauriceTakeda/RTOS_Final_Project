#include "angle_task.h"
#include <stdio.h>

void angle_task(int * angle, struct capsense_t *capsense, int size) {
    for(int i = 0; i < size; i++) {
        switch(capsense[i].slider_pos) {
            case farLeft:
                *angle -= capsense[i].time;
                break;
            case left:
                *angle -= capsense[i].time;
                break;
            case right:
                *angle += capsense[i].time;
                break;
            case farRight:
                *angle += capsense[i].time;
                break;
            default:
                break;
        }
    }
}