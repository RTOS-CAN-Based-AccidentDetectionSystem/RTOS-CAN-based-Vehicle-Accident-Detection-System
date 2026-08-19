/*
 * gsm.c
 *
 *  Created on: 30-Jul-2026
 *      Author: sunbeam
 */
/*
 * gsm.c
 *
 *  Created on: 30-Jul-2026
 *      Author: sunbeam
 */

#include "gsm.h"
#include <string.h>

extern UART_HandleTypeDef huart2;

/* Send any AT command */
void GSM_SendCommand(char *cmd)
{
    HAL_UART_Transmit(&huart2,
                      (uint8_t *)cmd,
                      strlen(cmd),
                      HAL_MAX_DELAY);
}

/* Send SMS */
void GSM_SendSMS(char *number, char *message)
{
    GSM_SendCommand("AT\r\n");
    HAL_Delay(1000);

    GSM_SendCommand("AT+CMGF=1\r\n");
    HAL_Delay(1000);

    HAL_UART_Transmit(&huart2,
                      (uint8_t *)"AT+CMGS=\"",
                      9,
                      HAL_MAX_DELAY);

    HAL_UART_Transmit(&huart2,
                      (uint8_t *)number,
                      strlen(number),
                      HAL_MAX_DELAY);

    HAL_UART_Transmit(&huart2,
                      (uint8_t *)"\"\r\n",
                      3,
                      HAL_MAX_DELAY);

    HAL_Delay(1000);

    HAL_UART_Transmit(&huart2,
                      (uint8_t *)message,
                      strlen(message),
                      HAL_MAX_DELAY);

    uint8_t end = 0x1A;   // Ctrl + Z
    HAL_UART_Transmit(&huart2,
                      &end,
                      1,
                      HAL_MAX_DELAY);

    HAL_Delay(5000);
}
