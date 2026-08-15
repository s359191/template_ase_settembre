/*********************************************************************************************************
**--------------File
*Info---------------------------------------------------------------------------------
** File name:           lib_RIT.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        atomic functions to be used by higher sw levels
** Correlated files:    lib_RIT.c, funct_RIT.c, IRQ_RIT.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "../button_EXINT/button.h"
#include "../joystick/joystick.h"
#include "LPC17xx.h"
#include "RIT.h"
#include <stdint.h>

extern volatile uint32_t pressed_button_0;
extern volatile uint32_t pressed_button_1;
extern volatile uint32_t pressed_button_2;

extern volatile uint32_t button_press_time_0;
extern volatile uint32_t button_press_time_1;
extern volatile uint32_t button_press_time_2;

extern volatile uint32_t joy_press_time_up;
extern volatile uint32_t joy_press_time_down;
extern volatile uint32_t joy_press_time_left;
extern volatile uint32_t joy_press_time_right;

extern volatile uint32_t pressed_joystick_up;
extern volatile uint32_t pressed_joystick_left;
extern volatile uint32_t pressed_joystick_right;
extern volatile uint32_t pressed_joystick_down;
extern volatile uint32_t pressed_joystick_select;
extern volatile uint32_t pressed_joystick_up_left;
extern volatile uint32_t pressed_joystick_up_right;
extern volatile uint32_t pressed_joystick_bottom_left;
extern volatile uint32_t pressed_joystick_bottom_right;

extern volatile uint8_t joystick_flag;

extern volatile uint8_t button_enabled_mask;
extern volatile uint8_t joystick_cardinal_enabled_mask;

/******************************************************************************
** Function name:		enable_RIT
**
** Descriptions:		Enable RIT
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void enable_RIT(void) {
  LPC_RIT->RICTRL |= (1 << 3);
  return;
}

/******************************************************************************
** Function name:		disable_RIT
**
** Descriptions:		Disable RIT
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void disable_RIT(void) {
  LPC_RIT->RICTRL &= ~(1 << 3);
  return;
}

/******************************************************************************
** Function name:		reset_RIT
**
** Descriptions:		Reset RIT
**
** parameters:			RIT number: 0 or 1
** Returned value:		None
**
******************************************************************************/
void reset_RIT(void) {
  LPC_RIT->RICOUNTER = 0; // Set count value to 0
  return;
}

uint32_t init_RIT(uint32_t RITInterval) {
  LPC_SC->PCLKSEL1 &= ~(3 << 26);
  LPC_SC->PCLKSEL1 |= (1 << 26); // RIT Clock = CCLK
  LPC_SC->PCONP |= (1 << 16);    // Enable power for RIT

  LPC_RIT->RICOMPVAL = RITInterval; // Set match value

  LPC_RIT->RICTRL = (1 << 1) | // Enable clear on match
                    (1 << 2);  // Enable timer for debug

  LPC_RIT->RICOUNTER = 0; // Set count value to 0

  NVIC_EnableIRQ(RIT_IRQn);

  pressed_button_0 = 0;
  pressed_button_1 = 0;
  pressed_button_2 = 0;

  button_press_time_0 = 0;
  button_press_time_1 = 0;
  button_press_time_2 = 0;

  joy_press_time_up = 0;
  joy_press_time_down = 0;
  joy_press_time_left = 0;
  joy_press_time_right = 0;

  pressed_joystick_up = 0;
  pressed_joystick_left = 0;
  pressed_joystick_right = 0;
  pressed_joystick_down = 0;
  pressed_joystick_select = 0;
  pressed_joystick_up_left = 0;
  pressed_joystick_up_right = 0;
  pressed_joystick_bottom_left = 0;
  pressed_joystick_bottom_right = 0;

  button_enabled_mask = BUTTON_MASK_ALL;
  joystick_cardinal_enabled_mask = JOY_CARDINAL_MASK_ALL;

  joystick_flag = 0;

  return (0);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
