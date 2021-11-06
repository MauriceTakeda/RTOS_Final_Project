
#ifndef __FUEL_CONTROL_TASK__
#define __FUEL_CONTROL_TASK__

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Run the fuel control task
///
/// @param[in] fuel_burn_rate
/// @param[in] btn_pressed
/// @param[in] btn_press_count
//----------------------------------------------------------------------------------------------------------------------------------
void fuel_control_task(int * fuel_burn_rate, int * btn_pressed, int btn_press_count);

#endif // __FUEL_CONTROL_TASK__