#include "joystick.h"

extern volatile uint8_t joystick_cardinal_enabled_mask;
extern volatile uint32_t pressed_joystick_up;
extern volatile uint32_t pressed_joystick_left;
extern volatile uint32_t pressed_joystick_right;
extern volatile uint32_t pressed_joystick_down;
extern volatile uint32_t pressed_joystick_up_left;
extern volatile uint32_t pressed_joystick_up_right;
extern volatile uint32_t pressed_joystick_bottom_left;
extern volatile uint32_t pressed_joystick_bottom_right;
extern volatile uint32_t joy_press_time_up;
extern volatile uint32_t joy_press_time_down;
extern volatile uint32_t joy_press_time_left;
extern volatile uint32_t joy_press_time_right;

static void _joystick_reset_cardinal_state(joystick_dir_t dir) {
  switch (dir) {
  case JOY_UP:
    pressed_joystick_up = 0;
    pressed_joystick_up_left = 0;
    pressed_joystick_up_right = 0;
    joy_press_time_up = 0;
    break;
  case JOY_DOWN:
    pressed_joystick_down = 0;
    pressed_joystick_bottom_left = 0;
    pressed_joystick_bottom_right = 0;
    joy_press_time_down = 0;
    break;
  case JOY_LEFT:
    pressed_joystick_left = 0;
    pressed_joystick_up_left = 0;
    pressed_joystick_bottom_left = 0;
    joy_press_time_left = 0;
    break;
  case JOY_RIGHT:
    pressed_joystick_right = 0;
    pressed_joystick_up_right = 0;
    pressed_joystick_bottom_right = 0;
    joy_press_time_right = 0;
    break;
  default:
    break;
  }
}

static void _joystick_set_cardinal_enabled(joystick_dir_t dir,
                                           uint8_t enabled) {
  if (dir > JOY_RIGHT)
    return;

  if (enabled)
    joystick_cardinal_enabled_mask |= (1U << dir);
  else {
    joystick_cardinal_enabled_mask &= ~(1U << dir);
    _joystick_reset_cardinal_state(dir);
  }
}

uint8_t joystick_is_cardinal_enabled(joystick_dir_t dir) {
  if (dir > JOY_RIGHT)
    return 0;
  return (joystick_cardinal_enabled_mask >> dir) & 1U;
}

void joystick_disable_up(void) { _joystick_set_cardinal_enabled(JOY_UP, 0); }
void joystick_disable_down(void) {
  _joystick_set_cardinal_enabled(JOY_DOWN, 0);
}
void joystick_disable_left(void) {
  _joystick_set_cardinal_enabled(JOY_LEFT, 0);
}
void joystick_disable_right(void) {
  _joystick_set_cardinal_enabled(JOY_RIGHT, 0);
}

void joystick_enable_up(void) { _joystick_set_cardinal_enabled(JOY_UP, 1); }
void joystick_enable_down(void) { _joystick_set_cardinal_enabled(JOY_DOWN, 1); }
void joystick_enable_left(void) { _joystick_set_cardinal_enabled(JOY_LEFT, 1); }
void joystick_enable_right(void) {
  _joystick_set_cardinal_enabled(JOY_RIGHT, 1);
}

void joystick_disable_all(void) {
  joystick_cardinal_enabled_mask = 0;
  pressed_joystick_up = 0;
  pressed_joystick_down = 0;
  pressed_joystick_left = 0;
  pressed_joystick_right = 0;
  pressed_joystick_up_left = 0;
  pressed_joystick_up_right = 0;
  pressed_joystick_bottom_left = 0;
  pressed_joystick_bottom_right = 0;
  joy_press_time_up = 0;
  joy_press_time_down = 0;
  joy_press_time_left = 0;
  joy_press_time_right = 0;
}

void joystick_enable_all(void) {
  joystick_cardinal_enabled_mask = JOY_CARDINAL_MASK_ALL;
}

void joystick_init(void) {
  // joy sel
  LPC_PINCON->PINSEL3 &= ~(3 << 18); // PIN mode GPIO (00b value per P1.25)
  LPC_GPIO1->FIODIR &=
      ~(1 << 25); // P1.25 Input (joysticks on PORT1 defined as Input)

  // joy down
  LPC_PINCON->PINSEL3 &= ~(3 << 20); // PIN mode GPIO (00b value per P1.24)
  LPC_GPIO1->FIODIR &=
      ~(1 << 26); // P1.25 Input (joysticks on PORT1 defined as Input)

  // joy left
  LPC_PINCON->PINSEL3 &= ~(3 << 22); // PIN mode GPIO (00b value per P1.23)
  LPC_GPIO1->FIODIR &=
      ~(1 << 27); // P1.25 Input (joysticks on PORT1 defined as Input)

  // joy right
  LPC_PINCON->PINSEL3 &= ~(3 << 24); // PIN mode GPIO (00b value per P1.22)
  LPC_GPIO1->FIODIR &=
      ~(1 << 28); // P1.25 Input (joysticks on PORT1 defined as Input)

  // joy up
  LPC_PINCON->PINSEL3 &= ~(3 << 26); // PIN mode GPIO (00b value per P1.21)
  LPC_GPIO1->FIODIR &=
      ~(1 << 29); // P1.25 Input (joysticks on PORT1 defined as Input)
}

/******************************************************************************
**                            End Of File
******************************************************************************/
