#include "button.h"

extern volatile uint8_t button_enabled_mask;
extern volatile uint32_t pressed_button_0;
extern volatile uint32_t pressed_button_1;
extern volatile uint32_t pressed_button_2;
extern volatile uint32_t button_press_time_0;
extern volatile uint32_t button_press_time_1;
extern volatile uint32_t button_press_time_2;

static void _button_reset_state(uint8_t button) {
  switch (button) {
  case BUTTON_0:
    pressed_button_0 = 0;
    button_press_time_0 = 0;
    break;
  case BUTTON_1:
    pressed_button_1 = 0;
    button_press_time_1 = 0;
    break;
  case BUTTON_2:
    pressed_button_2 = 0;
    button_press_time_2 = 0;
    break;
  default:
    break;
  }
}

static void _button_set_enabled(uint8_t button, uint8_t enabled) {
  if (button > BUTTON_2)
    return;

  if (enabled)
    button_enabled_mask |= (1U << button);
  else {
    button_enabled_mask &= ~(1U << button);
    _button_reset_state(button);
  }
}

int button_check_pressed(uint32_t pin_mask) {
  if ((LPC_GPIO2->FIOPIN & pin_mask) == 0U)
    return 1;
  return 0;
}

uint8_t BUTTON_is_enabled(uint8_t button) {
  if (button > BUTTON_2)
    return 0;
  return (button_enabled_mask >> button) & 1U;
}

void BUTTON_disable_0(void) { _button_set_enabled(BUTTON_0, 0); }
void BUTTON_disable_1(void) { _button_set_enabled(BUTTON_1, 0); }
void BUTTON_disable_2(void) { _button_set_enabled(BUTTON_2, 0); }

void BUTTON_enable_0(void) { _button_set_enabled(BUTTON_0, 1); }
void BUTTON_enable_1(void) { _button_set_enabled(BUTTON_1, 1); }
void BUTTON_enable_2(void) { _button_set_enabled(BUTTON_2, 1); }

void BUTTON_disable_all(void) {
  button_enabled_mask = 0;
  pressed_button_0 = 0;
  pressed_button_1 = 0;
  pressed_button_2 = 0;
  button_press_time_0 = 0;
  button_press_time_1 = 0;
  button_press_time_2 = 0;
}

void BUTTON_enable_all(void) { button_enabled_mask = BUTTON_MASK_ALL; }

void BUTTON_init(uint8_t button, uint8_t prio) {
  if (button == 0) {
    LPC_PINCON->PINSEL4 |= (1 << 20); /* External interrupt 0 pin selection */
    LPC_GPIO2->FIODIR &= ~(1 << 10);  /* PORT2.10 defined as input          */
    LPC_SC->EXTMODE |= 0x01;
    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_SetPriority(EINT0_IRQn, prio);
  }
  if (button == 1) {
    LPC_PINCON->PINSEL4 |= (1 << 22); /* External interrupt 0 pin selection */
    LPC_GPIO2->FIODIR &= ~(1 << 11);  /* PORT2.11 defined as input          */
    LPC_SC->EXTMODE |= 0x02;
    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_SetPriority(EINT1_IRQn, prio);
  }
  if (button == 2) {
    LPC_PINCON->PINSEL4 |= (1 << 24); /* External interrupt 0 pin selection */
    LPC_GPIO2->FIODIR &= ~(1 << 12);  /* PORT2.12 defined as input          */
    LPC_SC->EXTMODE |= 0x04;
    NVIC_EnableIRQ(EINT2_IRQn);
    NVIC_SetPriority(EINT2_IRQn, prio);
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
