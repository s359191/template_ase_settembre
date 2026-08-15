#ifndef __BUTTON_H
#define __BUTTON_H

#include "../common.h"
#include "../led/led.h"
#include "LPC17xx.h"

#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2

#define BUTTON_0_PIN (1U << 10) /* P2.10 */
#define BUTTON_1_PIN (1U << 11) /* P2.11 */
#define BUTTON_2_PIN (1U << 12) /* P2.12 */

/* ============================================================
 *  BUTTON PRESS HANDLERS (function-pointer based)
 *
 *  Buttons are debounced and classified inside the RIT handler
 *  (RIT/IRQ_RIT.c), exactly like the joystick. When a button event is
 *  recognised, the RIT calls the matching function pointer from the tables
 *  below. The actual press actions are defined in button_EXINT/IRQ_button.c.
 *
 *  The EXINT handlers only clear the pending interrupt flag; they
 *  do NOT participate in debounce or press classification.
 * ============================================================ */

typedef enum { BTN_0 = 0, BTN_1, BTN_2, BTN_COUNT } button_num_t;

typedef void (*button_callback_t)(void);

/* Release handler: receives how long the button was held, expressed in RIT
 * ticks (one tick == RIT_PERIOD_MS). Convert with TICKS_TO_* in common.h. */
typedef void (*button_hold_callback_t)(uint32_t held_ticks);

/* Defined in button_EXINT/IRQ_button.c. A NULL entry means "do nothing".
 *
 * Two complementary press models are available; populate only the tables you
 * need and leave the rest NULL:
 *
 *   MULTI-THRESHOLD (duration levels) -> as the button is held the hold time
 *                  crosses LVL1 (1 s) -> LVL2 (5 s) -> LVL3 (10 s); each level
 *                  handler fires once, mid-hold, the moment its threshold is
 *                  reached. A press released before LVL1 fires the SHORT (tap)
 *                  handler on release instead.
 *
 *   EDGE model -> button_press_handlers fires once the moment a (debounced)
 *                  press begins; button_release_handlers fires once when the
 *                  button is let go, handed the measured press-to-release time.
 *                  This is what you want when you must act on BOTH edges and/or
 *                  time the hold (e.g. countdown games). */
extern button_callback_t button_short_handlers[BTN_COUNT]; /* tap (< LVL1) */
extern button_callback_t button_lvl1_handlers[BTN_COUNT];  /* held >= LVL1 */
extern button_callback_t button_lvl2_handlers[BTN_COUNT];  /* held >= LVL2 */
extern button_callback_t button_lvl3_handlers[BTN_COUNT];  /* held >= LVL3 */
// extern button_callback_t button_lvl4_handlers[BTN_COUNT];  /* held >= LVL4 */
extern button_callback_t button_press_handlers[BTN_COUNT];
extern button_hold_callback_t button_release_handlers[BTN_COUNT];

/* Live press time, in RIT ticks, for each button. Updated by the RIT poll
 * (RIT/IRQ_RIT.c) every tick while held, reset to 0 on release; readable at
 * any moment. Convert with TICKS_TO_* in common.h. */
extern volatile uint32_t button_press_time_0;
extern volatile uint32_t button_press_time_1;
extern volatile uint32_t button_press_time_2;

#define BUTTON_MASK_ALL ((1U << BUTTON_0) | (1U << BUTTON_1) | (1U << BUTTON_2))

void BUTTON_init(uint8_t button, uint8_t prio);
int button_check_pressed(uint32_t pin_mask);

void BUTTON_disable_0(void);
void BUTTON_disable_1(void);
void BUTTON_disable_2(void);
void BUTTON_disable_all(void);
void BUTTON_enable_0(void);
void BUTTON_enable_1(void);
void BUTTON_enable_2(void);
void BUTTON_enable_all(void);

uint8_t BUTTON_is_enabled(uint8_t button);

void EINT0_IRQHandler(void);
void EINT1_IRQHandler(void);
void EINT2_IRQHandler(void);

/******************************************************************************
**                            End Of File
******************************************************************************/

#endif // __BUTTON_H
