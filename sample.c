#include "common.h"
#include "include.h"


// Private Variables
static volatile uint32_t Sys_Tick = 0;

// Local Functions Prototypes
static void InitSysTick(void);
void SysTick_Handler(void);
void Delay_SysTick(uint32_t SysTicks);

// Imported Assembly Function
extern void EXAM_FUNCTION_NAME(uint32_t VAR);

// Exported variables
volatile uint32_t VAR1;
volatile uint32_t VAR2;
volatile uint32_t VAR3;
volatile uint32_t VAR4;
volatile state_t state;
uint32_t VETT1[VETT1_N]; // size: VETT1_N
uint32_t vett1_idx;
uint8_t VETT2[VETT2_M]; // size: VETT2_M
uint8_t vett2_idx;

int main(void) {
  // Imperative Inits
  SystemInit();
  InitSysTick();

  /* VARIABLES INITIALIZATION */
  state = STATE_IDLE;
  memset(VETT1, 0, sizeof(VETT1));
  memset(VETT2, 0, sizeof(VETT2));
  vett1_idx = vett2_idx = 0;
  VAR1 = VAR2 = VAR3 = VAR4 = 0;

  /* PERIPHERALS INITIALIZATION */
  LED_init();
  joystick_init();
  // LCD_Initialization();
  // ADC_init();
  BUTTON_init(BUTTON_0, IRQ_PRIO_MEDIUM);
  BUTTON_init(BUTTON_1, IRQ_PRIO_MEDIUM);
  BUTTON_init(BUTTON_2, IRQ_PRIO_MEDIUM);

  // RIT WORKS WITH CLOCK = 100MHZ -> ONE INTERRUPT EVERY 50ms
  init_RIT(RIT_MS_TO_TICKS(RIT_PERIOD_MS));
  enable_RIT();

  /* ENTER LOW-POWER MODE */
  LPC_SC->PCON |= 0x1;         // PM0=1
  LPC_SC->PCON &= 0xFFFFFFFFD; // PM1=0

  /* ============================================================
   *   HIGH-LEVEL TIMER API  - already configures
   *   AND starts the timer, no separate enable_timer() needed.
   *   The "interrupt frequency" can be given in ms or in Hz.
   *
   *   Duration helpers (-> ms): SECONDS(s), MINUTES(m), HOURS(h)
   *   e.g. timer_single_ms(TIMER_0, MINUTES(2), TIM_PRIO_HIGH, cb);
   *
   *   Prescaler: the *_ms functions auto-pick a prescaler when the value is
   *   too large to fit in the 32-bit counter (>~171800 ms at 25 MHz), so you
   *   can pass long durations like HOURS(1) directly.
   * ============================================================ */

  /* --- Single-shot: fire once after a delay --- */
  // timer_single_ms(TIMER_0, SECONDS(1), TIM_PRIO_HIGH, timer0_callback);
  // // Fires after 1s, single shot timer_single_ms(TIMER_0, MINUTES(2),
  // TIM_PRIO_HIGH, timer0_callback);			// Fires after 2 minutes, single
  // shot timer_single_ms(TIMER_0, HOURS(1), TIM_PRIO_HIGH, timer0_callback);
  // // Fires after 1 hour, single shot timer_single_hz(TIMER_0, KHZ(1),
  // TIM_PRIO_HIGH, timer0_callback);					// Fires at 1 KHz
  // frequency
  // timer_single_nointr_ms(TIMER_0, SECONDS(1));
  // // hardware only, no IRQ

  /* --- Repetitive INFINITE  --- */
  // timer_periodic_ms(TIMER_1, SECONDS(1), TIM_PRIO_HIGH, timer1_callback);
  // // Fires every 1s timer_periodic_hz(TIMER_1, KHZ(1), TIM_PRIO_HIGH,
  // timer1_callback); 				// Fires with 1KHz frequency
  // timer_periodic_hz(TIMER_1, MHZ(1), TIM_PRIO_HIGH, timer1_callback);
  // // Fires with 1MHz frequency timer_periodic_nointr_ms(TIMER_1, MINUTES(1));
  // // hardware only, no IRQ

  /* --- Repetitive FINITE: runs only for turn_off_after_ms --- */

  // timer_periodic_finite_ms(TIMER_1, SECONDS(1), SECONDS(10), TIM_PRIO_HIGH,
  // timer1_callback);  // 1s tick for 10s timer_periodic_finite_hz(TIMER_1,
  // KHZ(1), MINUTES(1), TIM_PRIO_HIGH, timer1_callback);       // 1 kHz for 1
  // minute timer_periodic_finite_nointr_ms(TIMER_1, SECONDS(1), SECONDS(10));
  // // auto-stops, no user callback

  /* --- PWM (duty + period), interrupt drives the callback --- */
  // timer_pwm_ms(TIMER_2, 0.25, SECONDS(1), TIM_PRIO_HIGH, timer2_callback);
  // // 25% duty, 1s period timer_pwm_finite_hz(TIMER_3, 0.25, KHZ(1),
  // SECONDS(10), TIM_PRIO_HIGH, timer3_callback); 		// 25% duty, 1 kHz,
  // disable after 10s

  /* Frequency macros: KHZ / MHZ / GHZ. A real timer tops out
   * around the 25 MHz clock, so GHZ() is only useful to express the macro
   * itself, e.g. GHZ(1) == 1000000000. */

  /* ============================================================
   *   EASY FIELD-BASED DESCRIPTOR: set one named value
   *   per behaviour instead of OR-ing flags. timer_easy_setup()
   *   checks the combination and returns a tmr_status_t (TMR_OK == 0).
   *     shot = TMR_SHOT_SINGLE / TMR_SHOT_REPEAT
   *     run  = TMR_RUN_INFINITE / TMR_RUN_FINITE   (only if REPEAT)
   *     irq  = TMR_IRQ_OFF / TMR_IRQ_ON
   *     pwm  = TMR_PWM_OFF / TMR_PWM_ON  (pwm_on = ON fraction, e.g. 0.5)
   * ============================================================ */
  // Fire ONCE after 1s, with interrupt:
  /*
  timer_easy_config_t ecfg = {0};
  ecfg.timer_n      = TIMER_0;
  ecfg.shot         = TMR_SHOT_SINGLE;
  ecfg.run          = TMR_RUN_INFINITE;          // ignored for SINGLE
  ecfg.irq          = TMR_IRQ_ON;
  ecfg.pwm          = TMR_PWM_OFF;
  ecfg.period_ticks = TIM_MS_TO_TICKS_SIMPLE(SECONDS(1));
  ecfg.prio         = TIM_PRIO_HIGH;
  ecfg.callback     = timer0_callback;
  timer_easy_setup(&ecfg);
  */

  // 50% PWM, 1s period, repeating for 10s, with interrupt:
  /*
timer_easy_config_t pcfg = {0};
pcfg.timer_n      = TIMER_2;
pcfg.shot         = TMR_SHOT_REPEAT;
pcfg.run          = TMR_RUN_FINITE;            // stops after run_for_ms
pcfg.irq          = TMR_IRQ_ON;
pcfg.pwm          = TMR_PWM_ON;
pcfg.period_ticks = TIM_MS_TO_TICKS_SIMPLE(SECONDS(1));
pcfg.pwm_on       = 0.5f;                       // duty = 0.5 * period_ticks
pcfg.run_for_ms   = SECONDS(10);
pcfg.prio         = TIM_PRIO_HIGH;
pcfg.callback     = timer2_callback;
timer_easy_setup(&pcfg);
*/

  while (1) {

    /* PROGRAM FINITE STATE MACHINE */
    switch (state) {
    case STATE_IDLE:
      break;
    case STATE_RESET:
      break;
    }

    __ASM("wfi");
  }
}

/*
Shortcuts:
- Ctrl+H : replace
- F3 : find all occurrences
*/

/* Initialize SysTick using CMSIS Core_CM4 function */
static void InitSysTick(void) {
  SysTick_Config(SystemFrequency / 1000U); /* Configure the SysTick timer */
}
/* SysTick Interrupt Handler */
void SysTick_Handler(void) { Sys_Tick++; /* increment timer */ }
/* Delay Function based on SysTick Counter */
void Delay_SysTick(uint32_t SysTicks) {
  uint32_t DelayTimer_SysTick = Sys_GetTick() + SysTicks; /* Get End Tick */
  while (Sys_GetTick() < DelayTimer_SysTick)
    ; /* wait for timer */
}
/*Get Current Elapsed Ticks*/
uint32_t Sys_GetTick(void) { return Sys_Tick; }