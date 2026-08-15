#include "../common.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "LPC17xx.h"
#include "joystick.h"

/* ============================================================
 *  JOYSTICK ACTION HANDLERS
 *
 *  These run from the RIT handler (RIT/IRQ_RIT.c), which debounces
 *  each direction and drives two complementary models (see joystick.h):
 *    - multi-threshold (all directions): short (tap) / lvl1 / lvl2 / lvl3
 *    - edges (all directions):           press / release(held_ticks)
 *  Each event fires at most once per physical press. Keep these bodies
 *  clean: the hold time is measured in the RIT, and the live value for a
 *  cardinal is available via joy_press_time_up/down/left/right.
 * ============================================================ */

/* Imported variables */
extern volatile uint32_t VAR1;
extern volatile uint32_t VAR2;
extern volatile uint32_t VAR3;
extern volatile uint32_t VAR4;
extern volatile state_t state;
extern uint32_t VETT1[]; // size: VETT1_N
extern uint32_t vett1_idx;
extern uint8_t VETT2[]; // size: VETT2_M
extern uint8_t vett2_idx;
extern void EXAM_FUNCTION_NAME(uint32_t VAR);

/* ==================================
 * ======== CARDINAL PRESSES ========
 * ================================== */

/* ---------------- SHORT press (tap, < LOW) ---------------- */
static void joy_up_short(void) {
  /* >>> JOYSTICK SHORT PRESS UP */

  /* JOYSTICK SHORT PRESS UP >>> */
}

static void joy_down_short(void) {
  /* >>> JOYSTICK SHORT PRESS DOWN */

  /* JOYSTICK SHORT PRESS DOWN >>> */
}

static void joy_left_short(void) {
  /* >>> JOYSTICK SHORT PRESS LEFT */

  /* JOYSTICK SHORT PRESS LEFT >>> */
}

static void joy_right_short(void) {
  /* >>> JOYSTICK SHORT PRESS RIGHT */

  /* JOYSTICK SHORT PRESS RIGHT >>> */
}

static void joy_select_short(void) {
  /* >>> JOYSTICK SHORT PRESS SELECT */

  /* JOYSTICK SHORT PRESS SELECT >>> */
}

/* ---------------- THRESHOLD >= LVL1_PRESS_MS ----------------- */
static void joy_up_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD UP */

  /* JOYSTICK LVL1 HOLD UP >>> */
}

static void joy_down_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD DOWN */

  /* JOYSTICK LVL1 HOLD DOWN >>> */
}

static void joy_left_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD LEFT */

  /* JOYSTICK LVL1 HOLD LEFT >>> */
}

static void joy_right_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD RIGHT */

  /* JOYSTICK LVL1 HOLD RIGHT >>> */
}

static void joy_select_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD SELECT */

  /* JOYSTICK LVL1 HOLD SELECT >>> */
}

/* ---------------- THRESHOLD >= LVL2_PRESS_MS ----------------- */
static void joy_up_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD UP */

  /* JOYSTICK LVL2 HOLD UP >>> */
}

static void joy_down_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD DOWN */

  /* JOYSTICK LVL2 HOLD DOWN >>> */
}

static void joy_left_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD LEFT */

  /* JOYSTICK LVL2 HOLD LEFT >>> */
}

static void joy_right_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD RIGHT */

  /* JOYSTICK LVL2 HOLD RIGHT >>> */
}

static void joy_select_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD SELECT */

  /* JOYSTICK LVL2 HOLD SELECT >>> */
}

/* ---------------- THRESHOLD >= LVL3_PRESS_MS --------------- */
static void joy_up_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD UP */

  /* JOYSTICK LVL3 HOLD UP >>> */
}

static void joy_down_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD DOWN */

  /* JOYSTICK LVL3 HOLD DOWN >>> */
}

static void joy_left_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD LEFT */

  /* JOYSTICK LVL3 HOLD LEFT >>> */
}

static void joy_right_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD RIGHT */

  /* JOYSTICK LVL3 HOLD RIGHT >>> */
}

static void joy_select_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD SELECT */

  /* JOYSTICK LVL3 HOLD SELECT >>> */
}

/* ---------------- PRESS edge ----------------- */
/* Fired ONCE the instant a (debounced) press starts. */
static void joy_up_press(void) {
  /* >>> JOYSTICK PRESSED UP */

  /* JOYSTICK PRESSED UP >>> */
}

static void joy_down_press(void) {
  /* >>> JOYSTICK PRESSED DOWN */

  /* JOYSTICK PRESSED DOWN >>> */
}

static void joy_left_press(void) {
  /* >>> JOYSTICK PRESSED LEFT */

  /* JOYSTICK PRESSED LEFT >>> */
}

static void joy_right_press(void) {
  /* >>> JOYSTICK PRESSED RIGHT */

  /* JOYSTICK PRESSED RIGHT >>> */
}

static void joy_select_press(void) {
  /* >>> JOYSTICK PRESSED SELECT */

  /* JOYSTICK PRESSED SELECT >>> */
}

/* --------------- RELEASE edge ---------------- */
/* Fired ONCE on release, for ANY hold duration. held_ticks is the measured
 * press-to-release time in RIT ticks; convert with TICKS_TO_* (common.h). */
static void joy_up_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED UP (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED UP >>> */
}

static void joy_down_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED DOWN (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED DOWN >>> */
}

static void joy_left_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED LEFT (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED LEFT >>> */
}

static void joy_right_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED RIGHT (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED RIGHT >>> */
}

static void joy_select_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED SELECT (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED SELECT >>> */
}

/* ==================================
 * ======== DIAGONAL PRESSES ========
 * ================================== */

/* ---------------- SHORT press ---------------- */
static void joy_up_left_short(void) {
  /* >>> JOYSTICK SHORT PRESS UP-LEFT */

  /* JOYSTICK SHORT PRESS UP-LEFT >>> */
}

static void joy_up_right_short(void) {
  /* >>> JOYSTICK SHORT PRESS UP-RIGHT */

  /* JOYSTICK SHORT PRESS UP-RIGHT >>> */
}

static void joy_bottom_left_short(void) {
  /* >>> JOYSTICK SHORT PRESS BOTTOM-LEFT */

  /* JOYSTICK SHORT PRESS BOTTOM-LEFT >>> */
}

static void joy_bottom_right_short(void) {
  /* >>> JOYSTICK SHORT PRESS BOTTOM-RIGHT */

  /* JOYSTICK SHORT PRESS BOTTOM-RIGHT >>> */
}

/* ---------------- THRESHOLD >= LVL1_PRESS_MS ----------------- */
static void joy_up_left_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD UP-LEFT */

  /* JOYSTICK LVL1 HOLD UP-LEFT >>> */
}

static void joy_up_right_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD UP-RIGHT */

  /* JOYSTICK LVL1 HOLD UP-RIGHT >>> */
}

static void joy_bottom_left_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD BOTTOM-LEFT */

  /* JOYSTICK LVL1 HOLD BOTTOM-LEFT >>> */
}

static void joy_bottom_right_lvl1(void) {
  /* >>> JOYSTICK LVL1 HOLD BOTTOM-RIGHT */

  /* JOYSTICK LVL1 HOLD BOTTOM-RIGHT >>> */
}

/* ---------------- THRESHOLD >= LVL2_PRESS_MS ----------------- */
static void joy_up_left_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD UP-LEFT */

  /* JOYSTICK LVL2 HOLD UP-LEFT >>> */
}

static void joy_up_right_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD UP-RIGHT */

  /* JOYSTICK LVL2 HOLD UP-RIGHT >>> */
}

static void joy_bottom_left_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD BOTTOM-LEFT */

  /* JOYSTICK LVL2 HOLD BOTTOM-LEFT >>> */
}

static void joy_bottom_right_lvl2(void) {
  /* >>> JOYSTICK LVL2 HOLD BOTTOM-RIGHT */

  /* JOYSTICK LVL2 HOLD BOTTOM-RIGHT >>> */
}

/* ---------------- THRESHOLD >= LVL3_PRESS_MS --------------- */
static void joy_up_left_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD UP-LEFT */

  /* JOYSTICK LVL3 HOLD UP-LEFT >>> */
}

static void joy_up_right_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD UP-RIGHT */

  /* JOYSTICK LVL3 HOLD UP-RIGHT >>> */
}

static void joy_bottom_left_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD BOTTOM-LEFT */

  /* JOYSTICK LVL3 HOLD BOTTOM-LEFT >>> */
}

static void joy_bottom_right_lvl3(void) {
  /* >>> JOYSTICK LVL3 HOLD BOTTOM-RIGHT */

  /* JOYSTICK LVL3 HOLD BOTTOM-RIGHT >>> */
}

/* ---------------- PRESS edge ----------------- */
static void joy_up_left_press(void) {
  /* >>> JOYSTICK PRESSED UP-LEFT */

  /* JOYSTICK PRESSED UP-LEFT >>> */
}

static void joy_up_right_press(void) {
  /* >>> JOYSTICK PRESSED UP-RIGHT */

  /* JOYSTICK PRESSED UP-RIGHT >>> */
}

static void joy_bottom_left_press(void) {
  /* >>> JOYSTICK PRESSED BOTTOM-LEFT */

  /* JOYSTICK PRESSED BOTTOM-LEFT >>> */
}

static void joy_bottom_right_press(void) {
  /* >>> JOYSTICK PRESSED BOTTOM-RIGHT */

  /* JOYSTICK PRESSED BOTTOM-RIGHT >>> */
}

/* --------------- RELEASE edge ---------------- */
static void joy_up_left_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED UP-LEFT (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED UP-LEFT >>> */
}

static void joy_up_right_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED UP-RIGHT (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED UP-RIGHT >>> */
}

static void joy_bottom_left_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED BOTTOM-LEFT (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED BOTTOM-LEFT >>> */
}

static void joy_bottom_right_release(uint32_t held_ticks) {
  /* >>> JOYSTICK RELEASED BOTTOM-RIGHT (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* JOYSTICK RELEASED BOTTOM-RIGHT >>> */
}

/* Function-pointer tables consumed by the RIT handler.
 * Order MUST match joystick_dir_t:
 *   UP, DOWN, LEFT, RIGHT, SELECT, UP_LEFT, UP_RIGHT, BOTTOM_LEFT,
 * BOTTOM_RIGHT. Set an entry to NULL (0) if a direction/event should do
 * nothing. */
joystick_callback_t joystick_short_handlers[JOY_DIR_COUNT] = {
    joy_up_short,     /* JOY_UP     */
    joy_down_short,   /* JOY_DOWN   */
    joy_left_short,   /* JOY_LEFT   */
    joy_right_short,  /* JOY_RIGHT  */
    joy_select_short, /* JOY_SELECT */
    joy_up_left_short,     joy_up_right_short,
    joy_bottom_left_short, joy_bottom_right_short};

joystick_callback_t joystick_lvl1_handlers[JOY_DIR_COUNT] = {
    joy_up_lvl1,     /* JOY_UP     */
    joy_down_lvl1,   /* JOY_DOWN   */
    joy_left_lvl1,   /* JOY_LEFT   */
    joy_right_lvl1,  /* JOY_RIGHT  */
    joy_select_lvl1, /* JOY_SELECT */
    joy_up_left_lvl1,     joy_up_right_lvl1,
    joy_bottom_left_lvl1, joy_bottom_right_lvl1};

joystick_callback_t joystick_lvl2_handlers[JOY_DIR_COUNT] = {
    joy_up_lvl2,     /* JOY_UP     */
    joy_down_lvl2,   /* JOY_DOWN   */
    joy_left_lvl2,   /* JOY_LEFT   */
    joy_right_lvl2,  /* JOY_RIGHT  */
    joy_select_lvl2, /* JOY_SELECT */
    joy_up_left_lvl2,     joy_up_right_lvl2,
    joy_bottom_left_lvl2, joy_bottom_right_lvl2};

joystick_callback_t joystick_lvl3_handlers[JOY_DIR_COUNT] = {
    joy_up_lvl3,     /* JOY_UP     */
    joy_down_lvl3,   /* JOY_DOWN   */
    joy_left_lvl3,   /* JOY_LEFT   */
    joy_right_lvl3,  /* JOY_RIGHT  */
    joy_select_lvl3, /* JOY_SELECT */
    joy_up_left_lvl3,     joy_up_right_lvl3,
    joy_bottom_left_lvl3, joy_bottom_right_lvl3};

/* Edge handlers: all directions. */
joystick_callback_t joystick_press_handlers[JOY_DIR_COUNT] = {
    joy_up_press,     /* JOY_UP     */
    joy_down_press,   /* JOY_DOWN   */
    joy_left_press,   /* JOY_LEFT   */
    joy_right_press,  /* JOY_RIGHT  */
    joy_select_press, /* JOY_SELECT */
    joy_up_left_press,     joy_up_right_press,
    joy_bottom_left_press, joy_bottom_right_press};

joystick_hold_callback_t joystick_release_handlers[JOY_DIR_COUNT] = {
    joy_up_release,     /* JOY_UP     */
    joy_down_release,   /* JOY_DOWN   */
    joy_left_release,   /* JOY_LEFT   */
    joy_right_release,  /* JOY_RIGHT  */
    joy_select_release, /* JOY_SELECT */
    joy_up_left_release,     joy_up_right_release,
    joy_bottom_left_release, joy_bottom_right_release};

/******************************************************************************
**                            End Of File
******************************************************************************/
