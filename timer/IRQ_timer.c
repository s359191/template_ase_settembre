#include "../common.h"
#include "../include.h"
#include "timer.h"
#include <stdint.h>
#include <string.h>

/*
 * NOTE: Repetitive timers only in MR0!
 */

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

void timer0_callback(uint8_t irq_src) {

  if (irq_src & IR_MR0) {

    /* >>> TIMER 0 FIRES */

    /* TIMER 0 FIRES >>> */

  } else if (irq_src & IR_MR1) {

  } else if (irq_src & IR_MR2) {

  } else if (irq_src & IR_MR3) {
  }
}

void timer1_callback(uint8_t irq_src) {
  if (irq_src & IR_MR0) {

    /* >>> TIMER 1 FIRES */

    /* TIMER 1 FIRES >>> */

  } else if (irq_src & IR_MR1) {

  } else if (irq_src & IR_MR2) {

  } else if (irq_src & IR_MR3) {
  }
}

void timer2_callback(uint8_t irq_src) {
  if (irq_src & IR_MR0) {

    /* >>> TIMER 2 FIRES */

    /* TIMER 2 FIRES >>> */

  } else if (irq_src & IR_MR1) {

  } else if (irq_src & IR_MR2) {

  } else if (irq_src & IR_MR3) {
  }
}

void timer3_callback(uint8_t irq_src) {
  if (irq_src & IR_MR0) {

    /* >>> TIMER 3 FIRES */

    /* TIMER 3 FIRES >>> */

  } else if (irq_src & IR_MR1) {

  } else if (irq_src & IR_MR2) {

  } else if (irq_src & IR_MR3) {
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
