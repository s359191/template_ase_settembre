/*********************************************************************************************************
**--------------File
*Info---------------------------------------------------------------------------------
** File name:           funct_glcd.c
** Descriptions:        High level GLCD helpers (lines, shapes, numbers, text)
*built on top
**                      of the primitives provided by GLCD.c.
** Correlated files:    GLCD.c, GLCD.h, AsciiLib.c
** Note:                LCD_Initialization() must be called (display
*initialised/calibrated)
**                      before any of these functions are used.
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "AsciiLib.h"
#include "GLCD.h"
#include "LPC17xx.h"
#include <stdarg.h>
#include <stdio.h>


/* ============================================================
 * ====                     LINES                          ====
 * ============================================================ */

/* Horizontal segment: (x,y) -> (x + length - 1, y) */
void LCD_draw_hline(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
  if (length == 0)
    return;
  LCD_DrawLine(x, y, (uint16_t)(x + length - 1), y, color);
}

/* Vertical segment: (x,y) -> (x, y + length - 1) */
void LCD_draw_vline(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
  if (length == 0)
    return;
  LCD_DrawLine(x, y, x, (uint16_t)(y + length - 1), color);
}

/* ============================================================
 * ====                  RECTANGLES                        ====
 * ============================================================ */

/* Outline only, built from four edges. */
void LCD_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                   uint16_t color) {
  if (w == 0 || h == 0)
    return;

  LCD_draw_hline(x, y, w, color);                     /* top    */
  LCD_draw_hline(x, (uint16_t)(y + h - 1), w, color); /* bottom */
  LCD_draw_vline(x, y, h, color);                     /* left   */
  LCD_draw_vline((uint16_t)(x + w - 1), y, h, color); /* right  */
}

/* Solid rectangle, drawn as a stack of horizontal lines. */
void LCD_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                   uint16_t color) {
  uint16_t row;

  if (w == 0 || h == 0)
    return;

  for (row = 0; row < h; row++) {
    LCD_draw_hline(x, (uint16_t)(y + row), w, color);
  }
}

/* ============================================================
 * ====                    CIRCLES                         ====
 * ============================================================ */

/* Midpoint circle algorithm: draws the circle outline. */
void LCD_draw_circle(uint16_t xc, uint16_t yc, uint16_t r, uint16_t color) {
  int16_t x = (int16_t)r;
  int16_t y = 0;
  int16_t err = 1 - (int16_t)r;

  if (r == 0) {
    LCD_SetPoint(xc, yc, color);
    return;
  }

  while (x >= y) {
    LCD_SetPoint((uint16_t)(xc + x), (uint16_t)(yc + y), color);
    LCD_SetPoint((uint16_t)(xc - x), (uint16_t)(yc + y), color);
    LCD_SetPoint((uint16_t)(xc + x), (uint16_t)(yc - y), color);
    LCD_SetPoint((uint16_t)(xc - x), (uint16_t)(yc - y), color);
    LCD_SetPoint((uint16_t)(xc + y), (uint16_t)(yc + x), color);
    LCD_SetPoint((uint16_t)(xc - y), (uint16_t)(yc + x), color);
    LCD_SetPoint((uint16_t)(xc + y), (uint16_t)(yc - x), color);
    LCD_SetPoint((uint16_t)(xc - y), (uint16_t)(yc - x), color);

    y++;
    if (err < 0) {
      err += 2 * y + 1;
    } else {
      x--;
      err += 2 * (y - x) + 1;
    }
  }
}

/* Filled disc: for every scan line of the circle, draw a horizontal span. */
void LCD_fill_circle(uint16_t xc, uint16_t yc, uint16_t r, uint16_t color) {
  int16_t x = (int16_t)r;
  int16_t y = 0;
  int16_t err = 1 - (int16_t)r;

  if (r == 0) {
    LCD_SetPoint(xc, yc, color);
    return;
  }

  while (x >= y) {
    /* spans for the +/- y rows */
    LCD_DrawLine((uint16_t)(xc - x), (uint16_t)(yc + y), (uint16_t)(xc + x),
                 (uint16_t)(yc + y), color);
    LCD_DrawLine((uint16_t)(xc - x), (uint16_t)(yc - y), (uint16_t)(xc + x),
                 (uint16_t)(yc - y), color);
    /* spans for the +/- x rows */
    LCD_DrawLine((uint16_t)(xc - y), (uint16_t)(yc + x), (uint16_t)(xc + y),
                 (uint16_t)(yc + x), color);
    LCD_DrawLine((uint16_t)(xc - y), (uint16_t)(yc - x), (uint16_t)(xc + y),
                 (uint16_t)(yc - x), color);

    y++;
    if (err < 0) {
      err += 2 * y + 1;
    } else {
      x--;
      err += 2 * (y - x) + 1;
    }
  }
}

/* ============================================================
 * ====                 NUMBERS & TEXT                     ====
 * ============================================================ */

/* Signed integer. */
void LCD_draw_number(uint16_t x, uint16_t y, int32_t num, uint16_t color,
                     uint16_t bkColor) {
  char buf[12]; /* enough for -2147483648 + '\0' */
  sprintf(buf, "%d", (int)num);
  GUI_Text(x, y, (uint8_t *)buf, color, bkColor);
}

/* Floating point value with a configurable number of decimals. */
void LCD_draw_float(uint16_t x, uint16_t y, float value, uint8_t decimals,
                    uint16_t color, uint16_t bkColor) {
  char buf[32];
  sprintf(buf, "%.*f", (int)decimals, value);
  GUI_Text(x, y, (uint8_t *)buf, color, bkColor);
}

/* Plain string wrapper (GUI_Text expects a uint8_t*). */
void LCD_print_text(uint16_t x, uint16_t y, char *str, uint16_t color,
                    uint16_t bkColor) {
  GUI_Text(x, y, (uint8_t *)str, color, bkColor);
}

/* printf-style helper. */
void LCD_printf(uint16_t x, uint16_t y, uint16_t color, uint16_t bkColor,
                const char *fmt, ...) {
  char buf[64];
  va_list args;

  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  GUI_Text(x, y, (uint8_t *)buf, color, bkColor);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
