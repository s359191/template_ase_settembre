#ifndef __COMMON_H
#define __COMMON_H

/*
        Questo file contiene tutte le dichiarazioni comuni ai diversi
        file. Le variabili sono successivamente dichiarate dentro sample.c
*/
#include "inttypes.h"

/* =============================
 * ====== SYSTEM DEFINES =======
 * ============================= */

/*SystemFrequency defined in file system_LPC17xx.c initialized during
 * SystemInit()*/
extern uint32_t SystemFrequency;

// Get Current Ticks
uint32_t Sys_GetTick(void);

extern unsigned char led_value;

// IRQ priorities
typedef enum {
  IRQ_PRIO_LOWEST = 0U,
  IRQ_PRIO_LOW = 1U,
  IRQ_PRIO_MEDIUM = 2U,
  IRQ_PRIO_HIGH = 3U,
  IRQ_PRIO_HIGHEST = 4U
} irq_prio_t;

/* ==================================
 * ====== PERIPHERALS DEFINES =======
 * ================================== */

/* ---------- Human-friendly duration helpers (result in milliseconds)
 * ---------- Pass the result straight into any *_ms timer function, e.g.
 *     timer_single_ms(TIMER_0, MIN_TO_MS(2), TIM_PRIO_HIGH, cb);
 * Integer or fractional arguments both work (HOURS_TO_MS(0.5) -> 1800000).
 */
/* The +0.5 rounds to the nearest ms so fractional values are exact, e.g.
 * MINUTES(1.3) -> 78000 and MINUTES(0.7) -> 42000 (not 41999). */
#define NANO(nS) ((uint32_t)((nS) / 1000000.0 + 0.5)) // pS -> ms
#define MICRO(uS) ((uint32_t)((uS) / 1000.0 + 0.5))   // uS -> ms
#define SECONDS(S) ((uint32_t)((S)*1000.0 + 0.5))     // Seconds -> ms
#define MINUTES(M) ((uint32_t)((M)*60000.0 + 0.5))    // Minutes -> ms
#define HOURS(H) ((uint32_t)((H)*3600000.0 + 0.5))    // Hours -> ms

/* ---------- Human-friendly frequency helpers (result in Hz) ----------
 * Pass the result straight into any *_hz timer function, e.g.
 *     timer_periodic_hz(TIMER_0, KHZ_TO_HZ(1), TIM_PRIO_HIGH, cb);
 */
#define KHZ(K) ((uint32_t)((K)*1000.0 + 0.5))       // KHz -> Hz
#define MHZ(M) ((uint32_t)((M)*1000000.0 + 0.5))    // MHz -> Hz
#define GHZ(G) ((uint32_t)((G)*1000000000.0 + 0.5)) // GHz -> Hz

/** ---- RIT PERIOD ---- **/
#define RIT_PERIOD_MS 50 // 50 ms

/* ---- RIT-tick (one RIT period) -> real-time helpers ----
 * Convert a hold expressed in RIT ticks (as handed to a release handler,
 * or read from a live press-time global) into human units.
 * Resolution is one RIT tick (RIT_PERIOD_MS); ns uses uint64_t to avoid
 * overflow. */
#define TICKS_TO_MS(ticks) ((uint32_t)(ticks)*RIT_PERIOD_MS)
#define TICKS_TO_NS(ticks)                                                     \
  ((uint64_t)(ticks) * (uint64_t)RIT_PERIOD_MS * 1000000ULL)
#define TICKS_TO_TENTHS(ticks)                                                 \
  (TICKS_TO_MS(ticks) / 100U) /* decimi di secondo */
#define TICKS_TO_SECONDS(ticks) (TICKS_TO_MS(ticks) / 1000U)
#define TICKS_TO_MINUTES(ticks) (TICKS_TO_MS(ticks) / 60000U)
#define TICKS_TO_HOURS(ticks) (TICKS_TO_MS(ticks) / 3600000U)

/** ----  BUTTON/JOYSTICK PRESS THRESHOLDS ----
 * A press must last at least DEBOUNCE_PRESS_MS to count as real (debounce).
 * While the input is held, the hold time crosses LVL1 -> LVL2 -> LVL3 and each
 * level fires once. Joystick taps (release before LVL1) use
 * joystick_short_handlers. */
#define DEBOUNCE_PRESS_MS 100     // min hold to be a real press (debounce)
#define LVL1_PRESS_MS SECONDS(1)  // level 1: held >= 1  s
#define LVL2_PRESS_MS SECONDS(5)  // level 2: held >= 5  s
#define LVL3_PRESS_MS SECONDS(10) // level 3: held >= 10 s
// #define LVL4_PRESS_MS SECONDS(30) // level 4: held >= 30 s

/* ===================================
 * ====== MAIN PROGRAM DEFINES =======
 * =================================== */

#define BYTE0(x) ((uint8_t)((uint32_t)(x)&0x000000FFU))     // bits 7:0
#define BYTE1(x) ((uint8_t)(((uint32_t)(x) >> 8) & 0xFFU))  // bits 8:15
#define BYTE2(x) ((uint8_t)(((uint32_t)(x) >> 16) & 0xFFU)) // bits 16:23
#define BYTE3(x) ((uint8_t)(((uint32_t)(x) >> 24) & 0xFFU)) // bits 24:31


/** ---- N ---- **/
#define VETT1_N 8
#define VETT2_M 4

/** ---- STATE ---- **/
typedef enum { STATE_IDLE, STATE_RESET } state_t;

/******************************************************************************
**                            End Of File
******************************************************************************/

#endif // __COMMON_H
