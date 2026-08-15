#include "../joystick/joystick.h"
#include "../led/led.h"
#include "LPC17xx.h"
#include <stdio.h>

#ifndef __TIMER_H
#define __TIMER_H

/* ============================================================
 *  CLOCKING
 *  By default the timer clock source is PCLK = CCLK/4 = 25 MHz
 *  (the code never reprograms PCLKSEL for the timers, so the
 *  reset value of "divide by 4" is used).
 * ============================================================ */
#define TIM_DEFAULT_PCLK_HZ 25000000U

/* ---------- Time <-> ticks conversion ----------
 * PR is the prescale register value, FREQ_HZ is the timer input clock.
 * One timer tick lasts (PR+1)/FREQ_HZ seconds.
 */
#define TIM_MS_TO_TICKS(MS, PR, FREQ_HZ)                                       \
  ((uint32_t)(((uint64_t)(MS)) * ((FREQ_HZ) / 1000U) / ((PR) + 1U)))
#define TIM_HZ_TO_TICKS(HZ, PR, FREQ_HZ)                                       \
  ((uint32_t)((uint64_t)(FREQ_HZ) / ((PR) + 1U) / (HZ)))
#define TIM_TICKS_TO_MS(TICKS, PR, FREQ_HZ)                                    \
  ((uint32_t)(((uint64_t)(TICKS)) * ((PR) + 1U)) / ((FREQ_HZ) / 1000U))

/* Convenience versions assuming PR = 0 and the default 25 MHz timer clock */
#define TIM_MS_TO_TICKS_SIMPLE(MS)                                             \
  TIM_MS_TO_TICKS((MS), 0U, TIM_DEFAULT_PCLK_HZ)
#define TIM_HZ_TO_TICKS_SIMPLE(HZ)                                             \
  TIM_HZ_TO_TICKS((HZ), 0U, TIM_DEFAULT_PCLK_HZ)
#define TIM_TICKS_TO_MS_SIMPLE(TICKS)                                          \
  TIM_TICKS_TO_MS((TICKS), 0U, TIM_DEFAULT_PCLK_HZ)


/* ---------- Prescaler auto-selection ----------
 * The timer counter (TC) and match registers (MRx) are 32-bit. With PR = 0 and
 * the 25 MHz clock the counter overflows after ~171798 ms (~2 min 51 s). For
 * longer durations a prescaler is required: the *_ms helpers automatically pick
 * the smallest prescaler so the resulting tick count still fits in 32 bits.
 */
#define TIM_MAX_MR 0xFFFFFFFFUL

/* ---------- Match Control Register (MCR) per-match flags ----------
 * The MCR holds 3 bits per match register (MR0..MR3) at offsets 0,3,6,9.
 * These are the 3 bits for ONE match.
 */
#define TIMER_INTERRUPT_MR 1U /* generate an interrupt when MRx matches  */
#define TIMER_RESET_MR 2U     /* reset the timer counter when MRx matches */
#define TIMER_STOP_MR 4U      /* stop the timer when MRx matches          */

/* ---------- Interrupt Register (IR) source bits ---------- */
#define IR_MR0 1U
#define IR_MR1 2U
#define IR_MR2 4U
#define IR_MR3 8U

/* Priorities (NVIC) */
typedef enum {
  TIM_PRIO_HIGH = 3,
  TIM_PRIO_MED = 2,
  TIM_PRIO_LOW = 1,
  TIM_PRIO_IDLE = 0
} timer_prio_t;

/* Timers */
typedef enum {
  TIMER_0 = 0U,
  TIMER_1 = 1U,
  TIMER_2 = 2U,
  TIMER_3 = 3U
} timer_num_t;

/* Callback: receives the IR value so it can tell which match fired
 * (use the IR_MRx masks above). */
typedef void (*timer_callback_t)(uint8_t);

/* ============================================================
 *  GENERAL CONFIGURATION (used by the all-in-one timer_setup)
 * ============================================================ */

/* Behaviour flags: OR them together into timer_config_t.flags.
 *
 * NOTE on finite repetition: the LPC176x general-purpose timers have NO
 * hardware repeat counter, so counting "how many periods elapsed" can only
 * be done by counting match interrupts. Therefore TMR_FINITE always enables
 * the timer interrupt internally (even if TMR_INTERRUPT is not set); a NULL
 * callback is allowed if you only want the auto-stop behaviour.
 */
#define TMR_SINGLE_SHOT                                                        \
  0x00U /* fire once, then stop (default if not periodic) */
#define TMR_PERIODIC                                                           \
  0x01U /* reset counter on the period match -> repeats forever */
#define TMR_FINITE                                                             \
  0x02U /* repeat only for turn_off_after_ms, then auto-disable  */
#define TMR_INTERRUPT                                                          \
  0x04U /* enable NVIC interrupt and invoke the callback         */
#define TMR_PWM                                                                \
  0x08U /* two-match waveform: duty_ticks (MR0) + period (MR1)   */
#define TMR_NO_RESET                                                           \
  0x10U /* do NOT reset the counter on match (free running)      */
#define TMR_STOP_ON_MATCH                                                      \
  0x20U /* hardware-stop the counter on the (first) match        */

typedef struct {
  timer_num_t timer_n;        /* TIMER_0..TIMER_3                           */
  uint32_t flags;             /* OR of TMR_* above                          */
  uint32_t pclk_hz;           /* timer input clock; 0 => TIM_DEFAULT_PCLK_HZ */
  uint32_t prescale;          /* PR value (usually 0)                       */
  uint32_t period_ticks;      /* main period: MR0 normally, MR1 in PWM      */
  uint32_t duty_ticks;        /* PWM on-time: MR0 (only used with TMR_PWM)  */
  uint32_t turn_off_after_ms; /* total run time when TMR_FINITE is set      */
  timer_prio_t prio;          /* NVIC priority (when interrupt is enabled)  */
  timer_callback_t callback;  /* called from the IRQ (may be NULL)          */
} timer_config_t;

/* ---- Legacy low-level configuration structs (kept for manual use) ---- */
struct timer_configuration {
  uint8_t timer_n;
  uint32_t prescale;
  uint32_t mr0;
  uint32_t mr1;
  uint32_t mr2;
  uint32_t mr3;
  uint8_t configuration_mr0;
  uint8_t configuration_mr1;
  uint8_t configuration_mr2;
  uint8_t configuration_mr3;
};

struct repetitive_timer_configuration {
  uint8_t timer_n;
  uint32_t prescale;
  uint32_t mr0;
  uint32_t duration_ms; /* 0 => infinite */
};

struct finite_pwm_timer_configuration {
  uint8_t timer_n;
  uint32_t prescale;
  uint32_t mr0;
  uint8_t configuration_mr0;
  uint32_t mr1;
  uint8_t configuration_mr1;
  uint32_t duration_ms;
};

struct timer_info {
  int count;
  int max_count; /* 0 => infinite (handler never auto-disables) */
  timer_callback_t callback;
};

/* ================== Core Timer Functions ================== */
uint32_t timer_get_counter(timer_num_t timer_num);
void enable_timer(timer_num_t timer_n, timer_prio_t prio);
void enable_timer_nointr(timer_num_t timer_n);
void disable_timer(timer_num_t timer_n);
void reset_timer(timer_num_t timer_num);

/* ================== Low-Level Init Functions ================== */
void init_timer(struct timer_configuration *tm, timer_callback_t callback);
void init_timer_simplified(timer_num_t timer_n, uint32_t prescale, uint32_t mr0,
                           uint32_t mr1, uint8_t conf_mr0, uint8_t conf_mr1,
                           timer_callback_t callback);
void init_repetitive_timer(struct repetitive_timer_configuration *tm,
                           timer_callback_t callback);
void init_repetitive_timer_simplified(timer_num_t timer_n, uint32_t prescale,
                                      uint32_t mr0, uint32_t duration_ms,
                                      timer_callback_t callback);
void init_repetitive_timer_nointr(struct repetitive_timer_configuration *tm);
void init_infinite_timer_pwm(timer_num_t timer_n, float duty,
                             uint32_t period_ms, timer_callback_t callback);
void init_finite_timer_pwm(timer_num_t timer_n, float duty, uint32_t period_ms,
                           uint32_t duration_ms, timer_callback_t callback);

/* ============================================================
 *                THE ALL-IN-ONE GENERAL FUNCTION
 *  Configure AND start any timer from a single descriptor.
 *  Everything else below is a thin convenience wrapper on top.
 * ============================================================ */
void timer_setup(const timer_config_t *cfg);

/* ============================================================
 *            EASY FIELD-BASED CONFIGURATION
 *  Instead of OR-ing TMR_* flags into one bitmask, set each
 *  behaviour with its own named field. Every field has exactly
 *  two (sometimes more) self-documenting choices, so you just
 *  pick the value you want. timer_easy_setup() validates the
 *  combination (returns a tmr_status_t) and, if everything is
 *  consistent, builds a timer_config_t and starts the timer.
 * ============================================================ */

/* --- FIELD: single shot vs repetitive --- */
typedef enum {
  TMR_SHOT_SINGLE = 0, /* fire once, then stop                    */
  TMR_SHOT_REPEAT = 1  /* keep repeating every period             */
} tmr_shot_t;

/* --- FIELD: finite vs infinite run (only used when REPEAT) --- */
typedef enum {
  TMR_RUN_INFINITE = 0, /* repeat forever                         */
  TMR_RUN_FINITE = 1    /* repeat only for run_for_ms, then stop  */
} tmr_run_t;

/* --- FIELD: generate an interrupt (and call the callback) --- */
typedef enum {
  TMR_IRQ_OFF = 0, /* run purely in hardware, no callback         */
  TMR_IRQ_ON = 1   /* enable NVIC IRQ and invoke the callback     */
} tmr_irq_t;

/* --- FIELD: PWM output or plain timer --- */
typedef enum {
  TMR_PWM_OFF = 0, /* plain match timer (ignores pwm_on)          */
  TMR_PWM_ON = 1   /* two-match PWM: duty = pwm_on * period_ticks */
} tmr_pwm_t;

/* Return / validation codes from timer_easy_setup().
 * TMR_OK (0) means the timer was configured and started. */
typedef enum {
  TMR_OK = 0,
  TMR_ERR_NULL,               /* cfg pointer was NULL                       */
  TMR_ERR_TIMER_NUM,          /* timer_n not in TIMER_0..TIMER_3            */
  TMR_ERR_PERIOD_ZERO,        /* period_ticks must be > 0                   */
  TMR_ERR_FINITE_ON_SINGLE,   /* FINITE only makes sense with REPEAT        */
  TMR_ERR_FINITE_NO_DURATION, /* FINITE run needs run_for_ms > 0            */
  TMR_ERR_PWM_ON_RANGE,       /* pwm_on must be in (0.0, 1.0]               */
  TMR_ERR_PWM_SINGLE          /* PWM is inherently repetitive, not SINGLE   */
} tmr_status_t;

typedef struct {
  timer_num_t timer_n; /* TIMER_0..TIMER_3                                  */

  /* ---- the four independent behaviour fields ---- */
  tmr_shot_t shot; /* TMR_SHOT_SINGLE | TMR_SHOT_REPEAT                     */
  tmr_run_t run;   /* TMR_RUN_INFINITE | TMR_RUN_FINITE (only if REPEAT)   */
  tmr_irq_t irq;   /* TMR_IRQ_OFF | TMR_IRQ_ON                             */
  tmr_pwm_t pwm;   /* TMR_PWM_OFF | TMR_PWM_ON                             */

  /* ---- value fields ---- */
  uint32_t pclk_hz;      /* timer input clock; 0 => TIM_DEFAULT_PCLK_HZ    */
  uint32_t prescale;     /* PR value (usually 0)                           */
  uint32_t period_ticks; /* main period (use TIM_MS_TO_TICKS_SIMPLE(...))  */
  float pwm_on;          /* PWM ON fraction in (0,1]; duty = pwm_on*period */
  uint32_t run_for_ms;   /* total run time when run == TMR_RUN_FINITE      */
  timer_prio_t prio;     /* NVIC priority (used when irq == TMR_IRQ_ON)    */
  timer_callback_t callback; /* called from the IRQ (may be NULL)          */
} timer_easy_config_t;

/* Validate the field combination and, on success, start the timer.
 * Returns TMR_OK on success or a TMR_ERR_* code describing the first
 * inconsistency found (in which case the timer is NOT started). */
tmr_status_t timer_easy_setup(const timer_easy_config_t *cfg);

/* Human-readable description of a tmr_status_t (handy for debugging). */
const char *timer_status_str(tmr_status_t st);

/* ============================================================
 *                  HIGH-LEVEL CONVENIENCE API
 *  Standard variants assume the default 25 MHz timer clock.
 *  Every "frequency of generation" can be given in ms or in Hz.
 *  *_nointr variants run purely in hardware (no callback).
 * ============================================================ */

/* ---------------- Single-shot ---------------- */
void timer_single_ms(timer_num_t timer_n, uint32_t duration_ms,
                     timer_prio_t prio, timer_callback_t callback);
void timer_single_hz(timer_num_t timer_n, uint32_t frequency_hz,
                     timer_prio_t prio, timer_callback_t callback);
void timer_single_nointr_ms(timer_num_t timer_n, uint32_t duration_ms);
void timer_single_nointr_hz(timer_num_t timer_n, uint32_t frequency_hz);

/* ---------------- Repetitive (infinite) ---------------- */
void timer_periodic_ms(timer_num_t timer_n, uint32_t period_ms,
                       timer_prio_t prio, timer_callback_t callback);
void timer_periodic_hz(timer_num_t timer_n, uint32_t frequency_hz,
                       timer_prio_t prio, timer_callback_t callback);
void timer_periodic_nointr_ms(timer_num_t timer_n, uint32_t period_ms);
void timer_periodic_nointr_hz(timer_num_t timer_n, uint32_t frequency_hz);

/* ---------------- Repetitive finite (turn_off_after) ---------------- */
void timer_periodic_finite_ms(timer_num_t timer_n, uint32_t period_ms,
                              uint32_t turn_off_after_ms, timer_prio_t prio,
                              timer_callback_t callback);
void timer_periodic_finite_hz(timer_num_t timer_n, uint32_t frequency_hz,
                              uint32_t turn_off_after_ms, timer_prio_t prio,
                              timer_callback_t callback);
/* finite repetition needs interrupt counting; the *_nointr variants run the
 * timer IRQ only to count/auto-stop and use a NULL user callback. */
void timer_periodic_finite_nointr_ms(timer_num_t timer_n, uint32_t period_ms,
                                     uint32_t turn_off_after_ms);
void timer_periodic_finite_nointr_hz(timer_num_t timer_n, uint32_t frequency_hz,
                                     uint32_t turn_off_after_ms);

/* ---------------- PWM (duty + period) ---------------- */
void timer_pwm_ms(timer_num_t timer_n, float duty_cycle, uint32_t period_ms,
                  timer_prio_t prio, timer_callback_t callback);
void timer_pwm_hz(timer_num_t timer_n, float duty_cycle, uint32_t frequency_hz,
                  timer_prio_t prio, timer_callback_t callback);
void timer_pwm_finite_ms(timer_num_t timer_n, float duty_cycle,
                         uint32_t period_ms, uint32_t turn_off_after_ms,
                         timer_prio_t prio, timer_callback_t callback);
void timer_pwm_finite_hz(timer_num_t timer_n, float duty_cycle,
                         uint32_t frequency_hz, uint32_t turn_off_after_ms,
                         timer_prio_t prio, timer_callback_t callback);
void timer_pwm_nointr_ms(timer_num_t timer_n, float duty_cycle,
                         uint32_t period_ms);
void timer_pwm_nointr_hz(timer_num_t timer_n, float duty_cycle,
                         uint32_t frequency_hz);

/* ============================================================
 *  CUSTOM CLOCK helpers (when the timer PCLK is not 25 MHz).
 *  For full control of any combination, fill a timer_config_t
 *  and call timer_setup() directly.
 * ============================================================ */
void pclk_timer_single_ms(timer_num_t timer_n, uint32_t pclk_hz,
                          uint32_t duration_ms, timer_prio_t prio,
                          timer_callback_t callback);
void pclk_timer_periodic_ms(timer_num_t timer_n, uint32_t pclk_hz,
                            uint32_t period_ms, timer_prio_t prio,
                            timer_callback_t callback);
void pclk_timer_periodic_hz(timer_num_t timer_n, uint32_t pclk_hz,
                            uint32_t frequency_hz, timer_prio_t prio,
                            timer_callback_t callback);
void pclk_timer_periodic_finite_ms(timer_num_t timer_n, uint32_t pclk_hz,
                                   uint32_t period_ms,
                                   uint32_t turn_off_after_ms,
                                   timer_prio_t prio,
                                   timer_callback_t callback);

/* IRQ Callbacks (defined in IRQ_timer.c) */
void timer0_callback(uint8_t irq_src);
void timer1_callback(uint8_t irq_src);
void timer2_callback(uint8_t irq_src);
void timer3_callback(uint8_t irq_src);

#endif
/*****************************************************************************
**                            End Of File
******************************************************************************/
