#include "../common.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "button.h"

/* ============================================================
 *  BUTTON ACTION HANDLERS
 *
 *  These run from the RIT handler (RIT/IRQ_RIT.c), which debounces
 *  each button and drives two complementary models (see button.h):
 *    - multi-threshold: short (tap) / lvl1 / lvl2 / lvl3
 *    - edges:           press / release(held_ticks)
 *  Each event fires at most once per physical press. Keep these
 *  bodies clean: the hold time is measured in the RIT, and the live
 *  value is available via button_press_time_0/1/2 if you need it.
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

/* ---------------- SHORT press ---------------- */
static void btn0_short(void) {
  /* >>> BUTTON0 SHORT PRESS */

  /* BUTTON0 SHORT PRESS >>> */
}

static void btn1_short(void) {
  /* >>> BUTTON1 SHORT PRESS */

  /* BUTTON1 SHORT PRESS >>> */
}

static void btn2_short(void) {
  /* >>> BUTTON2 SHORT PRESS */

  /* BUTTON2 SHORT PRESS >>> */
}

/* ------------- THRESHOLD >= LVL1_PRESS_MS -------- */
/* Fired ONCE mid-hold the moment the hold time crosses LVL1_PRESS_MS. */
static void btn0_lvl1(void) {
  /* >>> BUTTON0 LVL1 HOLD */

  /* BUTTON0 LVL1 HOLD >>> */
}

static void btn1_lvl1(void) {
  /* >>> BUTTON1 LVL1 HOLD */

  /* BUTTON1 LVL1 HOLD >>> */
}

static void btn2_lvl1(void) {
  /* >>> BUTTON2 LVL1 HOLD */

  /* BUTTON2 LVL1 HOLD >>> */
}

/* ------------- THRESHOLD >= LVL2_PRESS_MS -------- */
static void btn0_lvl2(void) {
  /* >>> BUTTON0 LVL2 HOLD */

  /* BUTTON0 LVL2 HOLD >>> */
}

static void btn1_lvl2(void) {
  /* >>> BUTTON1 LVL2 HOLD */

  /* BUTTON1 LVL2 HOLD >>> */
}

static void btn2_lvl2(void) {
  /* >>> BUTTON2 LVL2 HOLD */

  /* BUTTON2 LVL2 HOLD >>> */
}

/* ------------- THRESHOLD >= LVL3_PRESS_MS ------ */
static void btn0_lvl3(void) {
  /* >>> BUTTON0 LVL3 HOLD */

  /* BUTTON0 LVL3 HOLD >>> */
}

static void btn1_lvl3(void) {
  /* >>> BUTTON1 LVL3 HOLD */

  /* BUTTON1 LVL3 HOLD >>> */
}

static void btn2_lvl3(void) {
  /* >>> BUTTON2 LVL3 HOLD */

  /* BUTTON2 LVL3 HOLD >>> */
}

/* ------------- THRESHOLD >= LVL4_PRESS_MS ------ */
// static void btn0_lvl4(void) {
//   /* >>> BUTTON0 LVL4 HOLD */
//
//   /* BUTTON0 LVL4 HOLD >>> */
// }
//
// static void btn1_lvl4(void) {
//   /* >>> BUTTON1 LVL4 HOLD */
//
//   /* BUTTON1 LVL4 HOLD >>> */
// }
//
// static void btn2_lvl4(void) {
//   /* >>> BUTTON2 LVL4 HOLD */
//
//   /* BUTTON2 LVL4 HOLD >>> */
// }

/* ---------------- PRESS edge ----------------- */
/* Fired ONCE the instant a (debounced) press starts. Use it to react to
 * the button going down */
static void btn0_press(void) {
  /* >>> BUTTON0 PRESSED */

  /* BUTTON0 PRESSED >>> */
}

static void btn1_press(void) {
  /* >>> BUTTON1 PRESSED */

  /* BUTTON1 PRESSED >>> */
}

static void btn2_press(void) {
  /* >>> BUTTON2 PRESSED */

  /* BUTTON2 PRESSED >>> */
}

/* --------------- RELEASE edge ---------------- */
/* Fired ONCE when the button is let go, for ANY hold duration. The argument
 * is the measured press-to-release time in RIT ticks; convert with TICKS_TO_*
 * (common.h). */
static void btn0_release(uint32_t held_ticks) {
  /* >>> BUTTON0 RELEASED (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* BUTTON0 RELEASED >>> */
}

static void btn1_release(uint32_t held_ticks) {
  /* >>> BUTTON1 RELEASED (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* BUTTON1 RELEASED >>> */
}

static void btn2_release(uint32_t held_ticks) {
  /* >>> BUTTON2 RELEASED (held_ticks = hold time in RIT ticks) */
  (void)held_ticks;
  /* BUTTON2 RELEASED >>> */
}

/* Function-pointer tables consumed by the RIT handler.
 * Order MUST match button_num_t: BTN_0, BTN_1, BTN_2.
 * Set an entry to NULL (0) if a button/event should do nothing. */
button_callback_t button_short_handlers[BTN_COUNT] = {
    btn0_short, /* BTN_0 */
    btn1_short, /* BTN_1 */
    btn2_short  /* BTN_2 */
};

button_callback_t button_lvl1_handlers[BTN_COUNT] = {
    btn0_lvl1, /* BTN_0 */
    btn1_lvl1, /* BTN_1 */
    btn2_lvl1  /* BTN_2 */
};

button_callback_t button_lvl2_handlers[BTN_COUNT] = {
    btn0_lvl2, /* BTN_0 */
    btn1_lvl2, /* BTN_1 */
    btn2_lvl2  /* BTN_2 */
};

button_callback_t button_lvl3_handlers[BTN_COUNT] = {
    btn0_lvl3, /* BTN_0 */
    btn1_lvl3, /* BTN_1 */
    btn2_lvl3  /* BTN_2 */
};

// button_callback_t button_lvl4_handlers[BTN_COUNT] = {
//     btn0_lvl4, /* BTN_0 */
//     btn1_lvl4, /* BTN_1 */
//     btn2_lvl4  /* BTN_2 */
// };

button_callback_t button_press_handlers[BTN_COUNT] = {
    btn0_press, /* BTN_0 */
    btn1_press, /* BTN_1 */
    btn2_press  /* BTN_2 */
};

button_hold_callback_t button_release_handlers[BTN_COUNT] = {
    btn0_release, /* BTN_0 */
    btn1_release, /* BTN_1 */
    btn2_release  /* BTN_2 */
};

/* ============================================================
 *  EXINT handlers: clear the pending interrupt flag only.
 *  Debounce + threshold/edge classification happen in the RIT.
 * ============================================================ */
void EINT0_IRQHandler(void) { LPC_SC->EXTINT |= (1 << 0); }

void EINT1_IRQHandler(void) { LPC_SC->EXTINT |= (1 << 1); }

void EINT2_IRQHandler(void) { LPC_SC->EXTINT |= (1 << 2); }

/******************************************************************************
**                            End Of File
******************************************************************************/
