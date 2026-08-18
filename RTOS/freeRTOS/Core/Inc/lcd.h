/*
 * lcd.h
 *
 *  Created on: 17-Aug-2026
 *      Author: sunbeam
 */

#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Line selectors used by lcd_puts() -- matches how main.c calls it:
 * lcd_puts(LCD_LINE1, "..."), lcd_puts(LCD_LINE2, "...") */
#define LCD_LINE1   0
#define LCD_LINE2   1

void lcd_init(void);
void lcd_clear(void);
void lcd_puts(uint8_t line, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
