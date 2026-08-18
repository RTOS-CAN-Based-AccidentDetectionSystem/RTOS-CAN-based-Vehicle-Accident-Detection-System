/* =====================================================================
 * gsm.c - SIM800/SIM900-class GSM module driver: AT init, SMS send,
 * and GPRS/HTTP POST for telemetry. All communication happens over
 * huart2 using blocking HAL_UART_Transmit/Receive (simple + fine for
 * a dedicated GSMTask/TelemetryTask that already blocks on a queue).
 *
 * If your module is SIM7600/SIM7000 (LTE-class), the GPRS bearer
 * bring-up differs (AT+CGDCONT / AT+NETOPEN instead of AT+SAPBR) --
 * see the comment above GSM_HTTPInit().
 * =====================================================================
 */
#include "gsm.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;

/* ---------------- Low-level AT command helper ----------------
 * Sends `cmd`, then polls the UART for up to `timeout_ms` looking for
 * `expect` in the response. Returns 1 as soon as `expect` is found, 0
 * if `timeout_ms` elapses without a match.
 *
 * This reads a byte at a time with a short per-byte timeout (20ms) and
 * checks for a match after every byte, instead of issuing one blocking
 * HAL_UART_Receive() for the full buffer length. The old approach
 * always blocked for the *entire* timeout_ms on every call, even when
 * the module replied in a few milliseconds, because it kept waiting
 * for a byte count the modem was never going to send. On the accident
 * path (GSM_SendSMS -> GSM_HTTPPost) those fixed waits stacked up to
 * ~35 seconds of pure dead time; this version returns as soon as the
 * expected response actually arrives and only uses the full timeout
 * as a worst-case ceiling for a genuinely slow/unresponsive module.
 * ---------------------------------------------------------------- */
static uint8_t GSM_SendCmd(const char *cmd, const char *expect, uint32_t timeout_ms)
{
    char rxbuf[160] = {0};
    uint16_t idx = 0;
    uint32_t start = HAL_GetTick();

    if (strlen(cmd) > 0)
    {
        HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), 1000);
    }

    while ((HAL_GetTick() - start) < timeout_ms && idx < sizeof(rxbuf) - 1)
    {
        uint32_t elapsed = HAL_GetTick() - start;
        uint32_t remaining = (elapsed < timeout_ms) ? (timeout_ms - elapsed) : 0;
        uint32_t chunk_timeout = (remaining > 20) ? 20 : remaining;
        uint8_t byte;

        if (chunk_timeout == 0)
        {
            break;
        }

        if (HAL_UART_Receive(&huart2, &byte, 1, chunk_timeout) == HAL_OK)
        {
            rxbuf[idx++] = (char)byte;
            rxbuf[idx] = '\0';

            if (strstr(rxbuf, expect) != NULL)
            {
                return 1;   /* found it -- no need to burn the rest of timeout_ms */
            }
        }
    }

    return (strstr(rxbuf, expect) != NULL);
}

/* ---------------- Public: basic init / SMS ---------------- */
void GSM_Init(void)
{
    GSM_SendCmd("AT\r\n", "OK", 2000);
    GSM_SendCmd("ATE0\r\n", "OK", 1000);              /* echo off */
    GSM_SendCmd("AT+CMGF=1\r\n", "OK", 1000);         /* SMS text mode */
}

uint8_t GSM_SendSMS(const char *number, const char *text)
{
    char cmd[48];
    uint8_t ok;

    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"\r\n", number);
    GSM_SendCmd(cmd, ">", 3000);   /* module prompts with '>' for message body */

    HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 3000);

    uint8_t ctrlZ = 0x1A;          /* Ctrl+Z terminates and sends the SMS */
    HAL_UART_Transmit(&huart2, &ctrlZ, 1, 1000);

    ok = GSM_SendCmd("", "+CMGS", 10000);
    return ok;
}

/* ---------------- Public: GPRS / HTTP telemetry ----------------
 * Sequence is written for SIM800/SIM900-class modules (AT+SAPBR bearer
 * + AT+HTTPxxx). For SIM7600/SIM7000 (LTE), replace the bearer setup
 * with:
 *   AT+CGDCONT=1,"IP","<apn>"
 *   AT+NETOPEN
 * and keep the same AT+HTTPxxx calls afterward.
 * ---------------------------------------------------------------- */
void GSM_HTTPInit(const char *apn)
{
    char cmd[96];

    GSM_SendCmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", "OK", 2000);

    snprintf(cmd, sizeof(cmd), "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apn);
    GSM_SendCmd(cmd, "OK", 2000);

    GSM_SendCmd("AT+SAPBR=1,1\r\n", "OK", 5000);   /* open GPRS bearer */
}

uint8_t GSM_HTTPPost(const char *url, const char *json_body)
{
    char cmd[192];
    uint16_t len = (uint16_t)strlen(json_body);
    uint8_t ok;

    GSM_SendCmd("AT+HTTPINIT\r\n", "OK", 2000);
    GSM_SendCmd("AT+HTTPPARA=\"CID\",1\r\n", "OK", 1000);

    snprintf(cmd, sizeof(cmd), "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
    GSM_SendCmd(cmd, "OK", 2000);

    GSM_SendCmd("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n", "OK", 1000);

    snprintf(cmd, sizeof(cmd), "AT+HTTPDATA=%u,10000\r\n", len);
    GSM_SendCmd(cmd, "DOWNLOAD", 2000);
    HAL_UART_Transmit(&huart2, (uint8_t *)json_body, len, 5000);

    ok = GSM_SendCmd("AT+HTTPACTION=1\r\n", "+HTTPACTION: 1,200", 10000);

    GSM_SendCmd("AT+HTTPTERM\r\n", "OK", 1000);

    return ok;
}
