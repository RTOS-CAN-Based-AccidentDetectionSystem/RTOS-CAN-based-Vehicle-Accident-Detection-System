/*
 * gsm.h
 *
 *  Created on: 17-Aug-2026
 *      Author: sunbeam
 */

#ifndef __GSM_H
#define __GSM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* ---- Basic AT / SMS functions ---- */
void    GSM_Init(void);
uint8_t GSM_SendSMS(const char *number, const char *text);

/* ---- GPRS / HTTP telemetry functions ---- */
void    GSM_HTTPInit(const char *apn);
uint8_t GSM_HTTPPost(const char *url, const char *json_body);

#ifdef __cplusplus
}
#endif

#endif /* __GSM_H */

