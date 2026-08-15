#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "../common.h"
#include "../led/led.h"
#include "LPC17xx.h"

#define JOYSTICK_UP 29
#define JOYSTICK_DOWN 26
#define JOYSTICK_LEFT 27
#define JOYSTICK_RIGHT 28
#define JOYSTICK_PRESS 25

#define FLAG_JOYSTICK_UP 1
#define FLAG_JOYSTICK_DOWN 2
#define FLAG_JOYSTICK_LEFT 4
#define FLAG_JOYSTICK_RIGHT 8
#define FLAG_JOYSTICK_SELECT 16

/* ============================================================
 *  JOYSTICK PRESS HANDLERS (function-pointer based)
 *
 *  The joystick has no entry in the interrupt vector table, so it
 *  is polled and debounced inside the RIT handler (RIT/IRQ_RIT.c).
 *  When a direction is recognised as a SHORT or LONG press, the RIT
 *  calls the matching function pointer from the tables below. The
 *  actual press actions are defined in joystick/IRQ_Joystick.c.
 * ============================================================ */

/* Index into the handler tables. Order MUST match the table init in
 * IRQ_Joystick.c and the calls in IRQ_RIT.c.
 * Diagonals: hold two adjacent cardinals together (e.g. UP+LEFT). */
typedef enum {
  JOY_UP = 0,
  JOY_DOWN,
  JOY_LEFT,
  JOY_RIGHT,
  JOY_SELECT,
  JOY_UP_LEFT,
  JOY_UP_RIGHT,
  JOY_BOTTOM_LEFT,  /* down + left  */
  JOY_BOTTOM_RIGHT, /* down + right */
  JOY_DIR_COUNT
} joystick_dir_t;

/* A press action: takes no argument, returns nothing. */
typedef void (*joystick_callback_t)(void);

/* Release action: receives the press-to-release hold time, in RIT ticks. */
typedef void (*joystick_hold_callback_t)(uint32_t held_ticks);

/* Defined in joystick/IRQ_Joystick.c. A NULL entry means "do nothing".
 *
 * Multi-threshold model (all directions): while held, the hold time crosses
 * LVL1 (1 s) -> LVL2 (5 s) -> LVL3 (10 s); each level handler fires once on
 * crossing. Released before LVL1 fires the SHORT (tap) handler on release.
 *
 * Edge model (all directions): joystick_press_handlers fires once on the
 * debounced press edge; joystick_release_handlers fires once on release,
 * handed the measured hold time. */
extern joystick_callback_t joystick_short_handlers[JOY_DIR_COUNT]; /* tap     */
extern joystick_callback_t joystick_lvl1_handlers[JOY_DIR_COUNT];  /* >= LVL1 */
extern joystick_callback_t joystick_lvl2_handlers[JOY_DIR_COUNT];  /* >= LVL2 */
extern joystick_callback_t joystick_lvl3_handlers[JOY_DIR_COUNT];  /* >= LVL3 */
// extern joystick_callback_t joystick_lvl4_handlers[JOY_DIR_COUNT]; /* >= LVL4
// */
extern joystick_callback_t joystick_press_handlers[JOY_DIR_COUNT];
extern joystick_hold_callback_t joystick_release_handlers[JOY_DIR_COUNT];

/* Live press time, in RIT ticks, for each cardinal direction. Updated by the
 * RIT poll every tick while held, reset to 0 on release. Convert with
 * TICKS_TO_* in common.h. */
extern volatile uint32_t joy_press_time_up;
extern volatile uint32_t joy_press_time_down;
extern volatile uint32_t joy_press_time_left;
extern volatile uint32_t joy_press_time_right;

#define JOY_CARDINAL_MASK_ALL                                                  \
  ((1U << JOY_UP) | (1U << JOY_DOWN) | (1U << JOY_LEFT) | (1U << JOY_RIGHT))

void joystick_init(void);
int joystick_check_dir(uint32_t dir);
int joystick_check_diag(uint32_t dir_a, uint32_t dir_b);

void joystick_disable_up(void);
void joystick_disable_down(void);
void joystick_disable_left(void);
void joystick_disable_right(void);
void joystick_disable_all(void);
void joystick_enable_up(void);
void joystick_enable_down(void);
void joystick_enable_left(void);
void joystick_enable_right(void);
void joystick_enable_all(void);

uint8_t joystick_is_cardinal_enabled(joystick_dir_t dir);

/******************************************************************************
**                            End Of File
******************************************************************************/

#endif // __JOYSTICK_H
