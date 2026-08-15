/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_adc.c
** Descriptions:        High level ADC helpers built on top of lib_adc.c / IRQ_adc.c
** Correlated files:    lib_adc.c, IRQ_adc.c, adc.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "LPC17xx.h"
#include "adc.h"

/* ============================================================
 * ====                 BLOCKING READ                      ====
 * ============================================================ */

/* Starts a conversion and waits (polling the DONE bit, ADGDR bit 31) for the
 * result. Self-contained: it does not rely on the ADC interrupt being enabled. */
uint16_t ADC_read_blocking(void) {
    uint32_t result;

    ADC_start_conversion();

    /* Wait until the "DONE" flag of the global data register is set */
    while (((LPC_ADC->ADGDR) & (1UL << 31)) == 0) {
        /* busy wait */
    }

    result = (LPC_ADC->ADGDR >> 4) & 0xFFF;   /* extract the 12-bit sample */
    return (uint16_t)result;
}

/* ============================================================
 * ====            CONVERSIONS / SCALING                   ====
 * ============================================================ */

/* Returns the most recent value produced by the ADC interrupt handler. */
uint16_t ADC_get_value(void) {
    return AD_current;
}

/* Latest sample expressed in millivolts (0 .. ADC_VREF_MV). */
uint16_t ADC_get_millivolts(void) {
    return (uint16_t)(((uint32_t)AD_current * ADC_VREF_MV) / ADC_RESOLUTION);
}

/* Latest sample expressed in volts (0.0 .. 3.3). */
float ADC_get_voltage(void) {
    return ((float)AD_current * ((float)ADC_VREF_MV / 1000.0f)) / (float)ADC_RESOLUTION;
}

/* Latest sample expressed as a 0 .. 100 percentage. */
uint8_t ADC_get_percent(void) {
    return (uint8_t)(((uint32_t)AD_current * 100U) / ADC_RESOLUTION);
}

/* Linearly maps the latest sample from [0, 4095] onto [out_min, out_max]. */
uint32_t ADC_map(uint32_t out_min, uint32_t out_max) {
    if (out_max >= out_min) {
        return out_min + ((uint32_t)AD_current * (out_max - out_min)) / ADC_RESOLUTION;
    }
    /* inverted range (out_min > out_max) */
    return out_min - ((uint32_t)AD_current * (out_min - out_max)) / ADC_RESOLUTION;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
