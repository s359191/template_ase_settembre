#include "joystick.h"

// Function use to check whether a direction of the joystick is pressed or not
int joystick_check_dir(uint32_t dir) {
  if ((LPC_GPIO1->FIOPIN & (1 << dir)) == 0)
    return 1;
  return 0;
}

// Returns 1 when both joystick directions are pressed (diagonal).
int joystick_check_diag(uint32_t dir_a, uint32_t dir_b) {
  return joystick_check_dir(dir_a) && joystick_check_dir(dir_b);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
