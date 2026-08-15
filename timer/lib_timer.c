#include "../common.h"
#include "timer.h"

// DEFAULT TIMER CLOCK SOURCE IS PCLK/4 = 25 MHz

struct timer_info timers_info[4];

/* ================== Internal helpers ================== */

/* Return the timer peripheral pointer and enable its power (PCONP). */
static LPC_TIM_TypeDef *_timer_get(timer_num_t timer_n) {
  switch (timer_n) {
  case TIMER_0:
    LPC_SC->PCONP |= (1 << 1);
    return (LPC_TIM_TypeDef *)LPC_TIM0_BASE;
  case TIMER_1:
    LPC_SC->PCONP |= (1 << 2);
    return (LPC_TIM_TypeDef *)LPC_TIM1_BASE;
  case TIMER_2:
    LPC_SC->PCONP |= (1 << 22);
    return (LPC_TIM_TypeDef *)LPC_TIM2_BASE;
  case TIMER_3:
    LPC_SC->PCONP |= (1 << 23);
    return (LPC_TIM_TypeDef *)LPC_TIM3_BASE;
  default:
    return (LPC_TIM_TypeDef *)LPC_TIM0_BASE;
  }
}

/* Convert a duration in ms into a (prescale, ticks) pair.
 *
 * "cycles" is the number of timer-input clock cycles the duration corresponds
 * to (i.e. the tick count when PR = 0). If that does not fit in a 32-bit match
 * register we pick the SMALLEST prescaler divisor (PR+1) so that
 *      ticks = cycles / (PR+1) <= TIM_MAX_MR
 * keeping the best possible resolution. When the value fits we return PR = 0.
 */
static void _ms_to_pr_ticks(uint32_t ms, uint32_t pclk_hz, uint32_t *pr,
                            uint32_t *ticks) {
  uint64_t cycles = (uint64_t)ms * (pclk_hz / 1000U);
  uint32_t div = 1U; /* div == PR + 1 */

  if (cycles > TIM_MAX_MR) {
    /* div = ceil(cycles / TIM_MAX_MR) -> smallest divisor that makes it fit */
    div = (uint32_t)((cycles + TIM_MAX_MR - 1U) / TIM_MAX_MR);
  }

  *pr = div - 1U;
  *ticks = (uint32_t)(cycles / div);
}

/* ================== Core Timer Functions ================== */

uint32_t timer_get_counter(timer_num_t timer_num) {
  switch (timer_num) {
  case TIMER_0:
    return LPC_TIM0->TC;
  case TIMER_1:
    return LPC_TIM1->TC;
  case TIMER_2:
    return LPC_TIM2->TC;
  case TIMER_3:
    return LPC_TIM3->TC;
  default:
    return 0;
  }
}

void enable_timer(timer_num_t timer_num, timer_prio_t priority) {
  if (timer_num == TIMER_0) {
    LPC_TIM0->TCR = 1;
    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_SetPriority(TIMER0_IRQn, priority);
  } else if (timer_num == TIMER_1) {
    LPC_TIM1->TCR = 1;
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_SetPriority(TIMER1_IRQn, priority);
  } else if (timer_num == TIMER_2) {
    LPC_TIM2->TCR = 1;
    NVIC_EnableIRQ(TIMER2_IRQn);
    NVIC_SetPriority(TIMER2_IRQn, priority);
  } else if (timer_num == TIMER_3) {
    LPC_TIM3->TCR = 1;
    NVIC_EnableIRQ(TIMER3_IRQn);
    NVIC_SetPriority(TIMER3_IRQn, priority);
  }
  return;
}

void enable_timer_nointr(timer_num_t timer_num) {
  if (timer_num == TIMER_0) {
    LPC_TIM0->TCR = 1;
    // No NVIC interrupt enabled
  } else if (timer_num == TIMER_1) {
    LPC_TIM1->TCR = 1;
  } else if (timer_num == TIMER_2) {
    LPC_TIM2->TCR = 1;
  } else if (timer_num == TIMER_3) {
    LPC_TIM3->TCR = 1;
  }
  return;
}

// RESET TIMER ALSO STOPS THE TIMER: NEED TO USE ENABLE_TIMER AGAIN!
void reset_timer(timer_num_t timer_num) {
  if (timer_num == TIMER_0)
    LPC_TIM0->TCR |= 0x02;
  else if (timer_num == TIMER_1)
    LPC_TIM1->TCR |= 0x02;
  else if (timer_num == TIMER_2)
    LPC_TIM2->TCR |= 0x02;
  else if (timer_num == TIMER_3)
    LPC_TIM3->TCR |= 0x02;
}

void disable_timer(timer_num_t timer_num) {
  if (timer_num == TIMER_0)
    LPC_TIM0->TCR = 0;
  else if (timer_num == TIMER_1)
    LPC_TIM1->TCR = 0;
  else if (timer_num == TIMER_2)
    LPC_TIM2->TCR = 0;
  else if (timer_num == TIMER_3)
    LPC_TIM3->TCR = 0;
  return;
}

/* ============================================================
 *                THE ALL-IN-ONE GENERAL FUNCTION
 * ============================================================ */
void timer_setup(const timer_config_t *cfg) {
  LPC_TIM_TypeDef *timer;
  uint32_t pclk;
  uint32_t mcr = 0;
  uint8_t intr, finite, pwm, periodic, count_intr;
  int max_count = 0;

  if (cfg == NULL)
    return;

  pclk = (cfg->pclk_hz != 0U) ? cfg->pclk_hz : TIM_DEFAULT_PCLK_HZ;

  intr = (cfg->flags & TMR_INTERRUPT) ? 1U : 0U;
  finite = (cfg->flags & TMR_FINITE) ? 1U : 0U;
  pwm = (cfg->flags & TMR_PWM) ? 1U : 0U;
  periodic = (cfg->flags & (TMR_PERIODIC | TMR_FINITE)) ? 1U : 0U;

  /* Finite repetition can only be measured by counting match interrupts,
   * so it always needs the timer IRQ even when the user wants no callback. */
  count_intr = finite ? 1U : 0U;

  timer = _timer_get(cfg->timer_n);

  timer->IR = timer->IR; /* clear any pending match flags  */
  timer->TCR = 0x02;     /* hold the counter in reset      */
  timer->PR = cfg->prescale;

  if (pwm) {
    /* MR0 = duty (on-time), MR1 = period (resets the counter). */
    timer->MR0 = cfg->duty_ticks;
    timer->MR1 = cfg->period_ticks;
    timer->MR2 = 0;
    timer->MR3 = 0;

    /* MR0: interrupt for the "duty" event and/or to count periods. */
    if (intr || count_intr)
      mcr |= (TIMER_INTERRUPT_MR << 0);
    /* MR1: interrupt for the "period" event, always reset to repeat. */
    if (intr)
      mcr |= (TIMER_INTERRUPT_MR << 3);
    mcr |= (TIMER_RESET_MR << 3);
  } else {
    uint8_t reset_on_match, stop_on_match;

    timer->MR0 = cfg->period_ticks;
    timer->MR1 = 0;
    timer->MR2 = 0;
    timer->MR3 = 0;

    reset_on_match = (cfg->flags & TMR_NO_RESET) ? 0U : 1U;
    /* Single-shot stops by default; periodic keeps running.            */
    stop_on_match = ((cfg->flags & TMR_STOP_ON_MATCH) || !periodic) ? 1U : 0U;

    if (intr || count_intr)
      mcr |= (TIMER_INTERRUPT_MR << 0);
    if (reset_on_match)
      mcr |= (TIMER_RESET_MR << 0);
    if (stop_on_match)
      mcr |= (TIMER_STOP_MR << 0);
  }

  timer->MCR = mcr;

  /* Compute how many periods a finite timer should run for. Done in 64-bit
   * (and honouring the prescaler) so a long turn_off_after_ms cannot overflow.
   */
  if (finite && cfg->turn_off_after_ms != 0U && cfg->period_ticks != 0U) {
    uint64_t total_ticks = ((uint64_t)cfg->turn_off_after_ms * (pclk / 1000U)) /
                           (cfg->prescale + 1U);
    max_count = (int)(total_ticks / cfg->period_ticks);
    if (max_count == 0)
      max_count = 1;
  } else {
    max_count = 0; /* 0 == infinite: the IRQ handler never auto-disables */
  }

  timers_info[cfg->timer_n].max_count = max_count;
  timers_info[cfg->timer_n].callback = (intr ? cfg->callback : NULL);
  timers_info[cfg->timer_n].count = 0;

  if (intr || count_intr)
    enable_timer(cfg->timer_n, cfg->prio);
  else
    enable_timer_nointr(cfg->timer_n);
}

/* ============================================================
 *            EASY FIELD-BASED CONFIGURATION
 * ============================================================ */

const char *timer_status_str(tmr_status_t st) {
  switch (st) {
  case TMR_OK:
    return "TMR_OK";
  case TMR_ERR_NULL:
    return "TMR_ERR_NULL: cfg pointer is NULL";
  case TMR_ERR_TIMER_NUM:
    return "TMR_ERR_TIMER_NUM: timer_n must be TIMER_0..TIMER_3";
  case TMR_ERR_PERIOD_ZERO:
    return "TMR_ERR_PERIOD_ZERO: period_ticks must be > 0";
  case TMR_ERR_FINITE_ON_SINGLE:
    return "TMR_ERR_FINITE_ON_SINGLE: FINITE needs TMR_SHOT_REPEAT";
  case TMR_ERR_FINITE_NO_DURATION:
    return "TMR_ERR_FINITE_NO_DURATION: FINITE needs run_for_ms > 0";
  case TMR_ERR_PWM_ON_RANGE:
    return "TMR_ERR_PWM_ON_RANGE: pwm_on must be in (0.0, 1.0]";
  case TMR_ERR_PWM_SINGLE:
    return "TMR_ERR_PWM_SINGLE: PWM is repetitive, use TMR_SHOT_REPEAT";
  default:
    return "TMR_ERR_UNKNOWN";
  }
}

/* Run all the consistency checks. Returns TMR_OK or the first problem. */
static tmr_status_t _easy_validate(const timer_easy_config_t *cfg) {
  if (cfg == NULL)
    return TMR_ERR_NULL;

  if ((int)cfg->timer_n < TIMER_0 || (int)cfg->timer_n > TIMER_3)
    return TMR_ERR_TIMER_NUM;

  if (cfg->period_ticks == 0U)
    return TMR_ERR_PERIOD_ZERO;

  /* FINITE only has meaning for a repetitive timer. */
  if (cfg->run == TMR_RUN_FINITE && cfg->shot != TMR_SHOT_REPEAT)
    return TMR_ERR_FINITE_ON_SINGLE;

  /* A finite run must say for how long. */
  if (cfg->shot == TMR_SHOT_REPEAT && cfg->run == TMR_RUN_FINITE &&
      cfg->run_for_ms == 0U)
    return TMR_ERR_FINITE_NO_DURATION;

  if (cfg->pwm == TMR_PWM_ON) {
    /* PWM continuously reloads on the period match -> it is repetitive. */
    if (cfg->shot == TMR_SHOT_SINGLE)
      return TMR_ERR_PWM_SINGLE;
    /* Duty fraction must be a sensible portion of the period. */
    if (cfg->pwm_on <= 0.0f || cfg->pwm_on > 1.0f)
      return TMR_ERR_PWM_ON_RANGE;
  }

  return TMR_OK;
}

tmr_status_t timer_easy_setup(const timer_easy_config_t *cfg) {
  tmr_status_t st;
  timer_config_t low = {0};

  /* On a bad combination we just refuse to start the timer and report the
   * reason through the return code. We deliberately do NOT printf() here:
   * this project has no stdio retargeting, so a printf() would invoke ARM
   * semihosting and HALT the core (making it look like every interrupt died).
   * Use timer_status_str(st) yourself if you have a working console. */
  st = _easy_validate(cfg);
  if (st != TMR_OK)
    return st;

  low.timer_n = cfg->timer_n;
  low.pclk_hz = cfg->pclk_hz;
  low.prescale = cfg->prescale;
  low.period_ticks = cfg->period_ticks;
  low.prio = cfg->prio;
  low.callback = cfg->callback;

  /* Translate the independent fields into the flag bitmask. */
  low.flags = 0U;

  if (cfg->pwm == TMR_PWM_ON) {
    low.flags |= TMR_PWM;
    /* The whole point: caller gives the ON fraction, we derive the ticks. */
    low.duty_ticks = (uint32_t)(cfg->pwm_on * (float)cfg->period_ticks);
  }

  if (cfg->shot == TMR_SHOT_REPEAT) {
    if (cfg->run == TMR_RUN_FINITE) {
      low.flags |= TMR_FINITE; /* timer_setup treats FINITE as periodic too */
      low.turn_off_after_ms = cfg->run_for_ms;
    } else {
      low.flags |= TMR_PERIODIC;
    }
  }
  /* TMR_SHOT_SINGLE => leave periodic bits clear (TMR_SINGLE_SHOT == 0). */

  if (cfg->irq == TMR_IRQ_ON)
    low.flags |= TMR_INTERRUPT;

  timer_setup(&low);
  return TMR_OK;
}

/* ================== Low-Level Init Functions ================== */

void init_infinite_timer_pwm(timer_num_t timer_n, float duty,
                             uint32_t period_ms, timer_callback_t callback) {
  uint32_t mr1 = TIM_MS_TO_TICKS_SIMPLE(period_ms);
  init_timer_simplified(timer_n, 0, (uint32_t)(duty * mr1), mr1,
                        TIMER_INTERRUPT_MR, TIMER_INTERRUPT_MR | TIMER_RESET_MR,
                        callback);
}

static void
_init_repetitive_timer_pwm(struct finite_pwm_timer_configuration *tm,
                           timer_callback_t callback) {
  LPC_TIM_TypeDef *timer = _timer_get(tm->timer_n);

  timer->IR = timer->IR; // reset irq flag
  timer->PR = tm->prescale;
  timer->MR0 = tm->mr0;
  timer->MR1 = tm->mr1;

  timer->MCR = ((tm->configuration_mr0 << 0) | (tm->configuration_mr1 << 3));

  timers_info[tm->timer_n].max_count =
      (tm->duration_ms) / TIM_TICKS_TO_MS_SIMPLE(tm->mr1);
  timers_info[tm->timer_n].callback = callback;
  timers_info[tm->timer_n].count = 0;
}

void init_finite_timer_pwm(timer_num_t timer_n, float duty, uint32_t period_ms,
                           uint32_t duration_ms, timer_callback_t callback) {
  uint32_t mr1 = TIM_MS_TO_TICKS_SIMPLE(period_ms);

  struct finite_pwm_timer_configuration fptm = {0};

  fptm.timer_n = timer_n;
  fptm.prescale = 0;
  fptm.configuration_mr0 = TIMER_INTERRUPT_MR;
  fptm.mr0 = (uint32_t)(duty * mr1);
  fptm.configuration_mr1 = TIMER_INTERRUPT_MR | TIMER_RESET_MR;
  fptm.mr1 = mr1;
  fptm.duration_ms = duration_ms;

  _init_repetitive_timer_pwm(&fptm, callback);
}

void init_timer_simplified(timer_num_t timer_n, uint32_t prescale, uint32_t mr0,
                           uint32_t mr1, uint8_t conf_mr0, uint8_t conf_mr1,
                           timer_callback_t callback) {
  struct timer_configuration tm = {0};

  tm.timer_n = timer_n;
  tm.prescale = prescale;
  tm.configuration_mr0 = conf_mr0;
  tm.configuration_mr1 = conf_mr1;
  tm.mr0 = mr0;
  tm.mr1 = mr1;

  init_timer(&tm, callback);
}

void init_timer(struct timer_configuration *tm, timer_callback_t callback) {
  LPC_TIM_TypeDef *timer = _timer_get(tm->timer_n);

  timer->IR = timer->IR; // reset irq flag
  timer->PR = tm->prescale;

  timer->MR0 = tm->mr0;
  timer->MR1 = tm->mr1;
  timer->MR2 = tm->mr2;
  timer->MR3 = tm->mr3;

  timer->MCR = ((tm->configuration_mr0 << 0) | (tm->configuration_mr1 << 3) |
                (tm->configuration_mr2 << 6) | (tm->configuration_mr3 << 9));

  timers_info[tm->timer_n].max_count = 0;
  timers_info[tm->timer_n].callback = callback;
  timers_info[tm->timer_n].count = 0;
}

void init_repetitive_timer_simplified(timer_num_t timer_n, uint32_t prescale,
                                      uint32_t mr0, uint32_t duration_ms,
                                      timer_callback_t callback) {
  struct repetitive_timer_configuration rtm = {0};

  rtm.timer_n = timer_n;
  rtm.prescale = prescale;
  rtm.mr0 = mr0;
  rtm.duration_ms = duration_ms;

  init_repetitive_timer(&rtm, callback);
}

/**
 * @params timer_configuration: timer config
 * @params duration_ms: total run time for a finite repetitive timer.
 *                      Set to 0 for an INFINITE repetitive timer (never
 *auto-stops).
 * @params callback: callback to be executed
 **/
void init_repetitive_timer(struct repetitive_timer_configuration *tm,
                           timer_callback_t callback) {
  LPC_TIM_TypeDef *timer = _timer_get(tm->timer_n);

  timer->IR = timer->IR; // reset irq flag
  timer->PR = tm->prescale;
  timer->MR0 = tm->mr0;

  timer->MCR = (((TIMER_INTERRUPT_MR | TIMER_RESET_MR) << 0));

  /* duration_ms == 0  ->  max_count == 0  ->  infinite (no auto-disable) */
  if (tm->duration_ms != 0U && tm->mr0 != 0U)
    timers_info[tm->timer_n].max_count =
        (tm->duration_ms) / TIM_TICKS_TO_MS_SIMPLE(tm->mr0);
  else
    timers_info[tm->timer_n].max_count = 0;
  timers_info[tm->timer_n].callback = callback;
  timers_info[tm->timer_n].count = 0;
}

/* No-interrupt version of repetitive timer init (for background operation
 * without IRQ) */
void init_repetitive_timer_nointr(struct repetitive_timer_configuration *tm) {
  LPC_TIM_TypeDef *timer = _timer_get(tm->timer_n);

  timer->IR = timer->IR; // reset irq flag
  timer->PR = tm->prescale;
  timer->MR0 = tm->mr0;

  // Only reset on match, NO interrupt
  timer->MCR = (TIMER_RESET_MR << 0);

  timers_info[tm->timer_n].max_count = 0;
  timers_info[tm->timer_n].callback = NULL;
  timers_info[tm->timer_n].count = 0;
}

/* ============================================================
 *                  HIGH-LEVEL CONVENIENCE API
 * ============================================================ */

/* ---------------- Single-shot ---------------- */

void timer_single_ms(timer_num_t timer_n, uint32_t duration_ms,
                     timer_prio_t prio, timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_SINGLE_SHOT | TMR_INTERRUPT;
  _ms_to_pr_ticks(duration_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_single_hz(timer_num_t timer_n, uint32_t frequency_hz,
                     timer_prio_t prio, timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_SINGLE_SHOT | TMR_INTERRUPT;
  cfg.period_ticks = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_single_nointr_ms(timer_num_t timer_n, uint32_t duration_ms) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_SINGLE_SHOT;
  _ms_to_pr_ticks(duration_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  timer_setup(&cfg);
}

void timer_single_nointr_hz(timer_num_t timer_n, uint32_t frequency_hz) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_SINGLE_SHOT;
  cfg.period_ticks = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  timer_setup(&cfg);
}

/* ---------------- Repetitive (infinite) ---------------- */

void timer_periodic_ms(timer_num_t timer_n, uint32_t period_ms,
                       timer_prio_t prio, timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PERIODIC | TMR_INTERRUPT;
  _ms_to_pr_ticks(period_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_periodic_hz(timer_num_t timer_n, uint32_t frequency_hz,
                       timer_prio_t prio, timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PERIODIC | TMR_INTERRUPT;
  cfg.period_ticks = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_periodic_nointr_ms(timer_num_t timer_n, uint32_t period_ms) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PERIODIC;
  _ms_to_pr_ticks(period_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  timer_setup(&cfg);
}

void timer_periodic_nointr_hz(timer_num_t timer_n, uint32_t frequency_hz) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PERIODIC;
  cfg.period_ticks = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  timer_setup(&cfg);
}

/* ---------------- Repetitive finite (turn_off_after) ---------------- */

void timer_periodic_finite_ms(timer_num_t timer_n, uint32_t period_ms,
                              uint32_t turn_off_after_ms, timer_prio_t prio,
                              timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_FINITE | TMR_INTERRUPT;
  _ms_to_pr_ticks(period_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  cfg.turn_off_after_ms = turn_off_after_ms;
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_periodic_finite_hz(timer_num_t timer_n, uint32_t frequency_hz,
                              uint32_t turn_off_after_ms, timer_prio_t prio,
                              timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_FINITE | TMR_INTERRUPT;
  cfg.period_ticks = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  cfg.turn_off_after_ms = turn_off_after_ms;
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_periodic_finite_nointr_ms(timer_num_t timer_n, uint32_t period_ms,
                                     uint32_t turn_off_after_ms) {
  /* No user callback, but TMR_FINITE still needs the IRQ to count/auto-stop. */
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_FINITE;
  _ms_to_pr_ticks(period_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  cfg.turn_off_after_ms = turn_off_after_ms;
  cfg.prio = TIM_PRIO_LOW;
  timer_setup(&cfg);
}

void timer_periodic_finite_nointr_hz(timer_num_t timer_n, uint32_t frequency_hz,
                                     uint32_t turn_off_after_ms) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_FINITE;
  cfg.period_ticks = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  cfg.turn_off_after_ms = turn_off_after_ms;
  cfg.prio = TIM_PRIO_LOW;
  timer_setup(&cfg);
}

/* ---------------- PWM (duty + period) ---------------- */

void timer_pwm_ms(timer_num_t timer_n, float duty_cycle, uint32_t period_ms,
                  timer_prio_t prio, timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PWM | TMR_INTERRUPT;
  _ms_to_pr_ticks(period_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  cfg.duty_ticks = (uint32_t)(duty_cycle * cfg.period_ticks);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_pwm_hz(timer_num_t timer_n, float duty_cycle, uint32_t frequency_hz,
                  timer_prio_t prio, timer_callback_t callback) {
  uint32_t period = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PWM | TMR_INTERRUPT;
  cfg.period_ticks = period;
  cfg.duty_ticks = (uint32_t)(duty_cycle * period);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_pwm_finite_ms(timer_num_t timer_n, float duty_cycle,
                         uint32_t period_ms, uint32_t turn_off_after_ms,
                         timer_prio_t prio, timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PWM | TMR_FINITE | TMR_INTERRUPT;
  _ms_to_pr_ticks(period_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  cfg.duty_ticks = (uint32_t)(duty_cycle * cfg.period_ticks);
  cfg.turn_off_after_ms = turn_off_after_ms;
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_pwm_finite_hz(timer_num_t timer_n, float duty_cycle,
                         uint32_t frequency_hz, uint32_t turn_off_after_ms,
                         timer_prio_t prio, timer_callback_t callback) {
  uint32_t period = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PWM | TMR_FINITE | TMR_INTERRUPT;
  cfg.period_ticks = period;
  cfg.duty_ticks = (uint32_t)(duty_cycle * period);
  cfg.turn_off_after_ms = turn_off_after_ms;
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void timer_pwm_nointr_ms(timer_num_t timer_n, float duty_cycle,
                         uint32_t period_ms) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PWM;
  _ms_to_pr_ticks(period_ms, TIM_DEFAULT_PCLK_HZ, &cfg.prescale,
                  &cfg.period_ticks);
  cfg.duty_ticks = (uint32_t)(duty_cycle * cfg.period_ticks);
  timer_setup(&cfg);
}

void timer_pwm_nointr_hz(timer_num_t timer_n, float duty_cycle,
                         uint32_t frequency_hz) {
  uint32_t period = TIM_HZ_TO_TICKS_SIMPLE(frequency_hz);
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PWM;
  cfg.period_ticks = period;
  cfg.duty_ticks = (uint32_t)(duty_cycle * period);
  timer_setup(&cfg);
}

/* ============================================================
 *                  CUSTOM PCLK helpers
 * ============================================================ */

void pclk_timer_single_ms(timer_num_t timer_n, uint32_t pclk_hz,
                          uint32_t duration_ms, timer_prio_t prio,
                          timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_SINGLE_SHOT | TMR_INTERRUPT;
  cfg.pclk_hz = pclk_hz;
  _ms_to_pr_ticks(duration_ms, pclk_hz, &cfg.prescale, &cfg.period_ticks);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void pclk_timer_periodic_ms(timer_num_t timer_n, uint32_t pclk_hz,
                            uint32_t period_ms, timer_prio_t prio,
                            timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PERIODIC | TMR_INTERRUPT;
  cfg.pclk_hz = pclk_hz;
  _ms_to_pr_ticks(period_ms, pclk_hz, &cfg.prescale, &cfg.period_ticks);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void pclk_timer_periodic_hz(timer_num_t timer_n, uint32_t pclk_hz,
                            uint32_t frequency_hz, timer_prio_t prio,
                            timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_PERIODIC | TMR_INTERRUPT;
  cfg.pclk_hz = pclk_hz;
  cfg.period_ticks = TIM_HZ_TO_TICKS(frequency_hz, 0, pclk_hz);
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

void pclk_timer_periodic_finite_ms(timer_num_t timer_n, uint32_t pclk_hz,
                                   uint32_t period_ms,
                                   uint32_t turn_off_after_ms,
                                   timer_prio_t prio,
                                   timer_callback_t callback) {
  timer_config_t cfg = {0};
  cfg.timer_n = timer_n;
  cfg.flags = TMR_FINITE | TMR_INTERRUPT;
  cfg.pclk_hz = pclk_hz;
  _ms_to_pr_ticks(period_ms, pclk_hz, &cfg.prescale, &cfg.period_ticks);
  cfg.turn_off_after_ms = turn_off_after_ms;
  cfg.prio = prio;
  cfg.callback = callback;
  timer_setup(&cfg);
}

/* ================== IRQ HANDLERS ================== */

/* A "period elapsed" is counted on the MR0 match: in every mode (periodic,
 * finite, PWM) MR0 fires exactly once per period, so counting it gives the
 * number of completed periods. max_count == 0 means infinite. */
void TIMER0_IRQHandler(void) {
  uint8_t irq_source;
  irq_source = LPC_TIM0->IR;

  if (irq_source) {
    if (timers_info[0].max_count != 0) {
      if (irq_source & IR_MR0) {
        timers_info[0].count++;
        if (timers_info[0].count >= timers_info[0].max_count) {
          disable_timer(TIMER_0);
        }
      }
    }
    if (timers_info[0].callback)
      timers_info[0].callback(irq_source);
  }
  // Reset IRQ Flag
  LPC_TIM0->IR = irq_source;
  return;
}

void TIMER1_IRQHandler(void) {
  uint8_t irq_source;
  irq_source = LPC_TIM1->IR;

  if (irq_source) {
    if (timers_info[1].max_count != 0) {
      if (irq_source & IR_MR0) {
        timers_info[1].count++;
        if (timers_info[1].count >= timers_info[1].max_count) {
          disable_timer(TIMER_1);
        }
      }
    }
    if (timers_info[1].callback)
      timers_info[1].callback(irq_source);
  }
  // Reset IRQ Flag
  LPC_TIM1->IR = irq_source;
  return;
}

void TIMER2_IRQHandler(void) {
  uint8_t irq_source;
  irq_source = LPC_TIM2->IR;

  if (irq_source) {
    if (timers_info[2].max_count != 0) {
      if (irq_source & IR_MR0) {
        timers_info[2].count++;
        if (timers_info[2].count >= timers_info[2].max_count) {
          disable_timer(TIMER_2);
        }
      }
    }
    if (timers_info[2].callback)
      timers_info[2].callback(irq_source);
  }
  // Reset IRQ Flag
  LPC_TIM2->IR = irq_source;
  return;
}

void TIMER3_IRQHandler(void) {
  uint8_t irq_source;
  irq_source = LPC_TIM3->IR;

  if (irq_source) {
    if (timers_info[3].max_count != 0) {
      if (irq_source & IR_MR0) {
        timers_info[3].count++;
        if (timers_info[3].count >= timers_info[3].max_count) {
          disable_timer(TIMER_3);
        }
      }
    }
    if (timers_info[3].callback)
      timers_info[3].callback(irq_source);
  }
  // Reset IRQ Flag
  LPC_TIM3->IR = irq_source;
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
