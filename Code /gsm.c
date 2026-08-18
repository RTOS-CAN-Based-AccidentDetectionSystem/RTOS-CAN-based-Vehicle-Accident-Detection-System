#include "gsm.h"
#include "usart.h"
#include <string.h>

void GSM_SendCommand(char *cmd)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 1000);
    HAL_Delay(1000);
}

void GSM_Init()
{
    GSM_SendCommand("AT");
    GSM_SendCommand("AT+CMGF=1");  
}

void GSM_SendSMS(char *number, char *message)
{
    char cmd[50];

    sprintf(cmd, "AT+CMGS=\"%s\"", number);
    GSM_SendCommand(cmd);

    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), 1000);

    uint8_t ctrlZ = 26;
    HAL_UART_Transmit(&huart2, &ctrlZ, 1, 1000);

    HAL_Delay(5000);
}
