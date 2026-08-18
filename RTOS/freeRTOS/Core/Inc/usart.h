/*
 * usart.h
 *
 *  Created on: 17-Aug-2026
 *      Author: sunbeam
 */

#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void MX_USART1_UART_Init(void);   /* GPS module */
void MX_USART2_UART_Init(void);   /* GSM module */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H */
