#ifndef __ADC_H
#define __ADC_H

#include <string.h>
#include <stdint.h>

/* ADC hardware reference (LandTiger / LPC1768): 12-bit, 3.3 V reference */
#define ADC_RESOLUTION   4095U   /* 2^12 - 1                              */
#define ADC_VREF_MV      3300U   /* reference voltage in millivolts       */

/* Last converted value, exponentially smoothed inside the ADC IRQ.
 * Defined in IRQ_adc.c. */
extern volatile uint16_t AD_current;

/* lib_adc.c */
void ADC_init (void);
void ADC_start_conversion (void);

/* IRQ_adc.c */
void ADC_IRQHandler(void);

/* ============================================================
 * ====            HIGH-LEVEL ADC HELPERS                  ====
 * ====              (funct_adc.c)                         ====
 * ============================================================ */

/**
 * @brief  Self-contained blocking read: starts a conversion, polls the
 *         hardware DONE bit and returns the raw 12-bit sample.
 * @note   Does not depend on the ADC interrupt; use it for one-shot reads.
 * @return raw value in [0, 4095]
 */
uint16_t ADC_read_blocking(void);

/**
 * @brief  Returns the latest value produced by the ADC interrupt
 *         (exponentially smoothed). Non-blocking.
 * @return raw value in [0, 4095]
 */
uint16_t ADC_get_value(void);

/**
 * @brief  Converts the latest ADC value to millivolts (0 .. ADC_VREF_MV).
 */
uint16_t ADC_get_millivolts(void);

/**
 * @brief  Converts the latest ADC value to volts (0.0 .. 3.3).
 */
float ADC_get_voltage(void);

/**
 * @brief  Returns the latest ADC value as a percentage (0 .. 100).
 */
uint8_t ADC_get_percent(void);

/**
 * @brief  Linearly maps the latest ADC value from [0, 4095] to
 *         [out_min, out_max].
 */
uint32_t ADC_map(uint32_t out_min, uint32_t out_max);

#endif // __ADC_H
