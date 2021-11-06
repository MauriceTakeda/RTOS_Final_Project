
#ifndef __ANGLE_TASK__
#define __ANGLE_TASK__

enum position {
    none = -1,
    farLeft,
    left,
    right,
    farRight,
};

struct capsense_t {
    int slider_pos;
    int time;
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Run the angle task
///
/// @param[in] angle
/// @param[in] capsense
/// @param[in] size
//----------------------------------------------------------------------------------------------------------------------------------
void angle_task(int * angle, struct capsense_t *capsense, int size);

#endif // __ANGLE_TASK__