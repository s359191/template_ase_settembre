/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_led.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        High level led management functions
** Correlated files:    lib_led.c, led.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "LPC17xx.h"
#include "led.h"


const unsigned long led_mask[] = { 1UL<<0, 1UL<<1, 1UL<<2, 1UL<<3, 1UL<<4, 1UL<<5, 1UL<<6, 1UL<<7 };

extern unsigned char led_value;

enum {
  LED_OFF = 0,
  LED_ON = 1
};

uint8_t LEDS_STATUS[LED_NUM] = {LED_OFF};


/* ==================================
 * ==== SINGLE LED ON/OFF/TOGGLE ====
 * ================================== */

/* Turns on the requested LED */
void LED_On(unsigned int num) {
    if (num >= LED_NUM) return;

    LPC_GPIO2->FIOPIN |= led_mask[num];
    led_value = LPC_GPIO2->FIOPIN;
    LEDS_STATUS[num] = LED_ON;
}

/* Turns off the requested LED */
void LED_Off(unsigned int num) {
    if (num >= LED_NUM) return;

    LPC_GPIO2->FIOPIN &= ~led_mask[num];
    led_value = LPC_GPIO2->FIOPIN;
    LEDS_STATUS[num] = LED_OFF;
}

/* Toggles the requested LED */
void LED_Toggle(unsigned int led) {
    if (led >= LED_NUM) return;

    if (LEDS_STATUS[led] == LED_ON) {
        LED_Off(led);
    } else {
        LED_On(led);
    }
}


/* ==================================
 * ====  MULTI LED ON/OFF/TOGGLE ====
 * ================================== */

/* Turns on all LEDs */
void LED_OnAll(void) {
    uint8_t led;
    for (led = 0; led < LED_NUM; led++) {
        LED_On(led);
    }
}

/* Turns off all LEDs */
void LED_OffAll(void) {
    uint8_t led;
    for (led = 0; led < LED_NUM; led++) {
        LED_Off(led);
    }
}

/* Toggles all LEDs */
void LED_ToggleAll(void) {
    uint8_t led;
    for (led = 0; led < LED_NUM; led++) {
        LED_Toggle(led);
    }
}


/* ======================================
 * ====  RANGE LED ON/OFF/TOGGLE     ====
 * ====================================== */

/* Turns on LEDs in the range [from_led_num, to_led_num] */
void LED_OnRange(uint8_t from_led_num, uint8_t to_led_num) {
    uint8_t i;

    if (to_led_num < from_led_num || to_led_num >= LED_NUM) return;

    for (i = from_led_num; i <= to_led_num; i++) {
        LED_On(i);
    }
}

/* Turns off LEDs in the range [from_led_num, to_led_num] */
void LED_OffRange(uint8_t from_led_num, uint8_t to_led_num) {
    uint8_t i;

    if (to_led_num < from_led_num || to_led_num >= LED_NUM) return;

    for (i = from_led_num; i <= to_led_num; i++) {
        LED_Off(i);
    }
}

/* Toggles LEDs in the range [from_led_num, to_led_num] */
void LED_ToggleRange(uint8_t from_led_num, uint8_t to_led_num) {
    uint8_t i;

    if (to_led_num < from_led_num || to_led_num >= LED_NUM) return;

    for (i = from_led_num; i <= to_led_num; i++) {
        LED_Toggle(i);
    }
}


/* ==============================
 * ====  MULTI LED FUNCTIONS ====
 * ============================== */

/* Outputs a value to LEDs */
void LED_Out(unsigned int value) {
    int i;

    for (i = 0; i < LED_NUM; i++) {
        if (value & (1 << i)) {
            LED_On(i);
        } else {
            LED_Off(i);
        }
    }
    led_value = LPC_GPIO2->FIOPIN;
}


/* Outputs a value to LEDs in range [from_led_num, to_led_num] */
void LED_Out_Range(unsigned int value, uint8_t from_led_num, uint8_t to_led_num) {
    int i, j;

    if (to_led_num < from_led_num || to_led_num >= LED_NUM) return;

    for (i = from_led_num, j = 0; i <= to_led_num; i++, j++) {
        if (value & (1 << j)) {
            LED_On(i);
        } else {
            LED_Off(i);
        }
    }
    led_value = LPC_GPIO2->FIOPIN;
}


/******************************************************************************
**                            End Of File
******************************************************************************/