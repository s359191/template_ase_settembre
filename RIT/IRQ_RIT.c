#include "../adc/adc.h"
#include "../button_EXINT/button.h"
#include "../common.h"
#include "../joystick/joystick.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "RIT.h"

/* Press thresholds expressed in RIT ticks (see common.h for the ms values). */
#define DEBOUNCE_PRESS_COUNT (DEBOUNCE_PRESS_MS / RIT_PERIOD_MS)
#define LVL1_PRESS_COUNT (LVL1_PRESS_MS / RIT_PERIOD_MS)
#define LVL2_PRESS_COUNT (LVL2_PRESS_MS / RIT_PERIOD_MS)
#define LVL3_PRESS_COUNT (LVL3_PRESS_MS / RIT_PERIOD_MS)
// #define LVL4_PRESS_COUNT (LVL4_PRESS_MS / RIT_PERIOD_MS)

/* Internal debounce/hold counters (one per input). */
volatile uint32_t pressed_button_0 = 0;
volatile uint32_t pressed_button_1 = 0;
volatile uint32_t pressed_button_2 = 0;

/* Live press time (in RIT ticks), readable from anywhere at any moment.
 * These mirror the hold duration while the input is down and are 0 when up. */
volatile uint32_t button_press_time_0 = 0;
volatile uint32_t button_press_time_1 = 0;
volatile uint32_t button_press_time_2 = 0;

volatile uint32_t joy_press_time_up = 0;
volatile uint32_t joy_press_time_down = 0;
volatile uint32_t joy_press_time_left = 0;
volatile uint32_t joy_press_time_right = 0;

volatile uint32_t pressed_joystick_up = 0;
volatile uint32_t pressed_joystick_left = 0;
volatile uint32_t pressed_joystick_right = 0;
volatile uint32_t pressed_joystick_down = 0;
volatile uint32_t pressed_joystick_select = 0;
volatile uint32_t pressed_joystick_up_left = 0;
volatile uint32_t pressed_joystick_up_right = 0;
volatile uint32_t pressed_joystick_bottom_left = 0;
volatile uint32_t pressed_joystick_bottom_right = 0;

volatile uint8_t button_enabled_mask = BUTTON_MASK_ALL;
volatile uint8_t joystick_cardinal_enabled_mask = JOY_CARDINAL_MASK_ALL;

volatile uint8_t joystick_flag = 0;

/* ============================================================
 *  SHARED PRESS STATE-MACHINE (debounce + multi-threshold + edges)
 *
 *  Counts how many RIT ticks an input has been held and drives every
 *  press event from that single counter, so the timing logic lives in
 *  ONE place and is reused by both buttons and the joystick.
 *
 *    released before DEBOUNCE_PRESS_COUNT ticks -> ignored (glitch)
 *    while held, crossing LVL1 / LVL2 / LVL3 -> on_lvl1/lvl2/lvl3 (once each)
 *    released in [DEBOUNCE, LVL1)              -> on_short (quick tap)
 *    debounced press begins                     -> on_press (edge)
 *    released after a real press                -> on_release(held_ticks)
 *
 *  Parameters:
 *    is_pressed  : 1 if the input reads as pressed this tick
 *    count       : per-input debounce/hold counter (ticks)
 *    press_time  : optional live hold-time mirror (NULL = not tracked)
 *    on_*        : per-event callbacks (NULL = ignore that event)
 * ============================================================ */
static void press_fsm(int is_pressed, volatile uint32_t *count,
                      volatile uint32_t *press_time, void (*on_short)(void),
                      void (*on_lvl1)(void), void (*on_lvl2)(void),
                      void (*on_lvl3)(void), void (*on_press)(void),
                      void (*on_release)(uint32_t)) {
  if (is_pressed) { /* currently pressed */
    (*count)++;
    if (press_time)
      *press_time = *count; /* publish live hold time */

    if (*count == DEBOUNCE_PRESS_COUNT && on_press)
      on_press(); /* press edge (debounced) */
    if (*count == LVL1_PRESS_COUNT && on_lvl1)
      on_lvl1(); /* crossed LVL1 */
    if (*count == LVL2_PRESS_COUNT && on_lvl2)
      on_lvl2(); /* crossed LVL2 */
    if (*count == LVL3_PRESS_COUNT && on_lvl3)
      on_lvl3(); /* crossed LVL3 */
    // if (*count == LVL4_PRESS_COUNT && on_lvl4)
    //   on_lvl4(); /* crossed LVL4 */
  } else {                                /* released / idle */
    if (*count >= DEBOUNCE_PRESS_COUNT) { /* it was a real press */
      if (on_release)
        on_release(*count); /* release edge, with measured hold time */
      if (*count < LVL1_PRESS_COUNT && on_short)
        on_short(); /* quick tap (never reached LVL1) */
    }
    *count = 0;
    if (press_time)
      *press_time = 0;
  }
}

/* Joystick: pure GPIO polling (no external interrupt to mask).
 * press_time is NULL for SELECT (live press time tracked for cardinals only).
 */
static void joystick_poll(uint32_t dir_pin, volatile uint32_t *count,
                          volatile uint32_t *press_time, joystick_dir_t idx) {
  if (idx <= JOY_RIGHT && !joystick_is_cardinal_enabled(idx)) {
    *count = 0;
    if (press_time)
      *press_time = 0;
    return;
  }
  press_fsm(joystick_check_dir(dir_pin), count, press_time,
            joystick_short_handlers[idx], joystick_lvl1_handlers[idx],
            joystick_lvl2_handlers[idx], joystick_lvl3_handlers[idx],
            joystick_press_handlers[idx], joystick_release_handlers[idx]
            /* , joystick_lvl4_handlers[idx] */);
}

/* Diagonal: both component directions must be held together.
 * No edge/press-time tracking for diagonals (cardinals only). */
static void joystick_diag_poll(uint32_t dir_a, uint32_t dir_b,
                               volatile uint32_t *count, joystick_dir_t idx) {
  press_fsm(joystick_check_diag(dir_a, dir_b), count, 0,
            joystick_short_handlers[idx], joystick_lvl1_handlers[idx],
            joystick_lvl2_handlers[idx], joystick_lvl3_handlers[idx],
            joystick_press_handlers[idx], joystick_release_handlers[idx]
            /* , joystick_lvl4_handlers[idx] */);
}

static int _joystick_cardinal_pressed(joystick_dir_t dir, uint32_t pin) {
  if (!joystick_is_cardinal_enabled(dir))
    return 0;
  return joystick_check_dir(pin);
}

/* Button: same model as the joystick.
 * EXINT only clears its pending flag; GPIO is polled here. */
static void button_poll(uint32_t pin_mask, volatile uint32_t *count,
                        volatile uint32_t *press_time, button_num_t idx) {
  if (!BUTTON_is_enabled((uint8_t)idx)) {
    *count = 0;
    *press_time = 0;
    return;
  }
  press_fsm(button_check_pressed(pin_mask), count, press_time,
            button_short_handlers[idx], button_lvl1_handlers[idx],
            button_lvl2_handlers[idx], button_lvl3_handlers[idx],
            button_press_handlers[idx], button_release_handlers[idx]
            /* , button_lvl4_handlers[idx] */);
}

void RIT_IRQHandler(void) {

  /**********************************
   *           JOYSTICK             *
   **********************************/
  {
    int up = _joystick_cardinal_pressed(JOY_UP, JOYSTICK_UP);
    int down = _joystick_cardinal_pressed(JOY_DOWN, JOYSTICK_DOWN);
    int left = _joystick_cardinal_pressed(JOY_LEFT, JOYSTICK_LEFT);
    int right = _joystick_cardinal_pressed(JOY_RIGHT, JOYSTICK_RIGHT);

    /* Diagonals first: both enabled cardinals must be held together. */
    if (up && left)
      joystick_diag_poll(JOYSTICK_UP, JOYSTICK_LEFT, &pressed_joystick_up_left,
                         JOY_UP_LEFT);
    if (up && right)
      joystick_diag_poll(JOYSTICK_UP, JOYSTICK_RIGHT,
                         &pressed_joystick_up_right, JOY_UP_RIGHT);
    if (down && left)
      joystick_diag_poll(JOYSTICK_DOWN, JOYSTICK_LEFT,
                         &pressed_joystick_bottom_left, JOY_BOTTOM_LEFT);
    if (down && right)
      joystick_diag_poll(JOYSTICK_DOWN, JOYSTICK_RIGHT,
                         &pressed_joystick_bottom_right, JOY_BOTTOM_RIGHT);

    /* While a diagonal is active, clear cardinal counters (and their live
     * press times) so a later release cannot also fire the components. */
    if (up && left) {
      pressed_joystick_up = 0;
      pressed_joystick_left = 0;
      joy_press_time_up = 0;
      joy_press_time_left = 0;
    }
    if (up && right) {
      pressed_joystick_up = 0;
      pressed_joystick_right = 0;
      joy_press_time_up = 0;
      joy_press_time_right = 0;
    }
    if (down && left) {
      pressed_joystick_down = 0;
      pressed_joystick_left = 0;
      joy_press_time_down = 0;
      joy_press_time_left = 0;
    }
    if (down && right) {
      pressed_joystick_down = 0;
      pressed_joystick_right = 0;
      joy_press_time_down = 0;
      joy_press_time_right = 0;
    }

    /* Cardinals only when not part of an active diagonal. */
    if (!(up && left) && !(up && right))
      joystick_poll(JOYSTICK_UP, &pressed_joystick_up, &joy_press_time_up,
                    JOY_UP);
    else {
      pressed_joystick_up = 0;
      joy_press_time_up = 0;
    }
    if (!(down && left) && !(down && right))
      joystick_poll(JOYSTICK_DOWN, &pressed_joystick_down, &joy_press_time_down,
                    JOY_DOWN);
    else {
      pressed_joystick_down = 0;
      joy_press_time_down = 0;
    }
    if (!(up && left) && !(down && left))
      joystick_poll(JOYSTICK_LEFT, &pressed_joystick_left, &joy_press_time_left,
                    JOY_LEFT);
    else {
      pressed_joystick_left = 0;
      joy_press_time_left = 0;
    }
    if (!(up && right) && !(down && right))
      joystick_poll(JOYSTICK_RIGHT, &pressed_joystick_right,
                    &joy_press_time_right, JOY_RIGHT);
    else {
      pressed_joystick_right = 0;
      joy_press_time_right = 0;
    }

    /* SELECT: no press-time tracking (cardinals only). */
    joystick_poll(JOYSTICK_PRESS, &pressed_joystick_select, 0, JOY_SELECT);
  }

  /**********************************
   *           BUTTONS              *
   **********************************/
  button_poll(BUTTON_0_PIN, &pressed_button_0, &button_press_time_0, BTN_0);
  button_poll(BUTTON_1_PIN, &pressed_button_1, &button_press_time_1, BTN_1);
  button_poll(BUTTON_2_PIN, &pressed_button_2, &button_press_time_2, BTN_2);

  /**********************************
   *        ADC CONVERSION          *
   **********************************/

  // ADC_start_conversion();

  LPC_RIT->RICTRL |= 0x1;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
