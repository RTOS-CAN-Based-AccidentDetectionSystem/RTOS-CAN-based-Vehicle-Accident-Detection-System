#include "main.h"
#include "gsm.h"

extern UART_HandleTypeDef huart2;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_USART2_UART_Init();

    GSM_Init();

    while (1)
    {
        GSM_SendSMS("+918080190968", "Test SMS from STM32");
        HAL_Delay(15000);
    }
}
