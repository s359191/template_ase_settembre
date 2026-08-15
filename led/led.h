																			 /*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           timer.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_led, funct_led .c files
** Correlated files:    lib_led.c, funct_led.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#ifndef __LED_H
#define __LED_H

// total number of available leds, don't change
// 0 is the rightmost led called led11 on the board
// 8 is the leftmost led called led4 on the board
#define LED_NUM 8

/* lib_led */
void LED_init(void);
void LED_deinit(void);

/* ==================================
 * ==== SINGLE LED ON/OFF/TOGGLE ====
 * ================================== */

/* Turns on the requested LED */
void LED_On(unsigned int num);
/* Turns off the requested LED */
void LED_Off(unsigned int num);
/* Toggles the requested LED */
void LED_Toggle(unsigned int led);

/* ==================================
 * ====  MULTI LED ON/OFF/TOGGLE ====
 * ================================== */

/* Turns on all LEDs */
void LED_OnAll(void);
/* Turns off all LEDs */
void LED_OffAll(void);
/* Toggles all LEDs */
void LED_ToggleAll(void);

/* ======================================
 * ====  RANGE LED ON/OFF/TOGGLE     ====
 * ====================================== */

/* Turns on LEDs in the range [from_led_num, to_led_num] */
void LED_OnRange(uint8_t from_led_num, uint8_t to_led_num);
/* Turns off LEDs in the range [from_led_num, to_led_num] */
void LED_OffRange(uint8_t from_led_num, uint8_t to_led_num);
/* Toggles LEDs in the range [from_led_num, to_led_num] */
void LED_ToggleRange(uint8_t from_led_num, uint8_t to_led_num);

/* ==============================
 * ====  MULTI LED FUNCTIONS ====
 * ============================== */

/* Outputs a value to LEDs (binary encoding over all LEDs) */
void LED_Out(unsigned int value);

/**
*@brief display a decimal value with binary encoding on a range of leds
*@param value the value to display on the range of leds
*@param from_led_num led position for range start
*@param to_led_num led position for range end
*@note LED_NUM >= to_led_num > from_led_num >= 0
*/
void LED_Out_Range(unsigned int value, uint8_t from_led_num, uint8_t to_led_num);

/******************************************************************************
**                            End Of File
******************************************************************************/

#endif // __LED_H
