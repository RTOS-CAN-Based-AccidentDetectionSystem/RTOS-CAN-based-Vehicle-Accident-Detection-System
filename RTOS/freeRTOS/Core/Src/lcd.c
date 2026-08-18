/*
 * lcd.c
 *
 *  Created on: 17-Aug-2026
 *      Author: sunbeam
 */


/* =====================================================================
 * lcd.c - HD44780-compatible 16x2 character LCD driver, 4-bit parallel
 * mode, bit-banged over plain GPIO (no I2C backpack -- I2C1 is already
 * used by the MPU6050).
 *
 * Wiring (see main.h for the exact pin macros):
 *   RS  -> LCD_RS_PIN     R/W -> tie to GND directly (write-only driver)
 *   EN  -> LCD_EN_PIN
 *   D4..D7 -> LCD_D4_PIN..LCD_D7_PIN   (D0-D3 left unconnected, 4-bit mode)
 *   VSS -> GND, VDD -> 5V (or 3.3V module), V0 -> contrast pot wiper
 *   A/K -> backlight +/- if your module has one
 *
 * This is a from-scratch, driver-only implementation -- swap it out if
 * you already have a tested lcd.c from a previous project; the function
 * signatures (lcd_init, lcd_clear, lcd_puts) are what main.c expects.
 * =====================================================================
 */
#include "lcd.h"

/* ---------------- Low-level pin helpers ---------------- */
static void LCD_Delay(uint32_t us)
{
    /* Simple busy-wait; HAL_Delay() only has ms resolution, and LCD
     * command timing needs microseconds. Good enough at typical
     * SysTick/core clock speeds for a 16x2 character LCD. */
    volatile uint32_t count = us * 8; /* rough calibration, adjust if needed */
    while (count--) { __NOP(); }
}

static void LCD_PulseEnable(void)
{
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_Delay(1);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_Delay(100);
}

/* Sends the upper nibble of `nibble` (bits 4-7) out on D4-D7 */
static void LCD_WriteNibble(uint8_t nibble)
{
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (nibble & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (nibble & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (nibble & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (nibble & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    LCD_PulseEnable();
}

static void LCD_SendByte(uint8_t data, uint8_t is_data)
{
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, is_data ? GPIO_PIN_SET : GPIO_PIN_RESET);

    LCD_WriteNibble(data & 0xF0);          /* high nibble first */
    LCD_WriteNibble((data << 4) & 0xF0);   /* then low nibble   */

    LCD_Delay(50);
}

static void LCD_Cmd(uint8_t cmd)  { LCD_SendByte(cmd, 0); }
static void LCD_Data(uint8_t ch)  { LCD_SendByte(ch, 1); }

static void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();   /* all LCD pins are on GPIOB in this pin map */

    GPIO_InitStruct.Pin = LCD_RS_PIN | LCD_EN_PIN | LCD_D4_PIN
                         | LCD_D5_PIN | LCD_D6_PIN | LCD_D7_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
}

/* ---------------- Public API ---------------- */
void lcd_init(void)
{
    LCD_GPIO_Init();

    HAL_Delay(50);   /* wait for LCD power-on internal reset */

    /* HD44780 4-bit init sequence (per datasheet) */
    LCD_WriteNibble(0x30); HAL_Delay(5);
    LCD_WriteNibble(0x30); LCD_Delay(150);
    LCD_WriteNibble(0x30); LCD_Delay(150);
    LCD_WriteNibble(0x20); LCD_Delay(150);   /* switch to 4-bit mode */

    LCD_Cmd(0x28);   /* function set: 4-bit, 2 line, 5x8 font */
    LCD_Cmd(0x0C);   /* display ON, cursor OFF, blink OFF */
    LCD_Cmd(0x06);   /* entry mode: increment, no shift */
    LCD_Cmd(0x01);   /* clear display */
    HAL_Delay(2);    /* clear command needs ~1.6ms */
}

void lcd_clear(void)
{
    LCD_Cmd(0x01);
    HAL_Delay(2);
}

void lcd_puts(uint8_t line, const char *str)
{
    /* DDRAM start addresses for a standard 16x2 HD44780 */
    uint8_t addr = (line == LCD_LINE2) ? 0xC0 : 0x80;
    LCD_Cmd(addr);

    while (*str)
    {
        LCD_Data((uint8_t)*str++);
    }
}
