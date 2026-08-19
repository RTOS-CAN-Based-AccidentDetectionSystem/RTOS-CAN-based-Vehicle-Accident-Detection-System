/*
 * gsm.h
 *
 *  Created on: 30-Jul-2026
 *      Author: sunbeam
 */

#ifndef INC_GSM_H_
#define INC_GSM_H_

#ifndef GSM_H
#define GSM_H
#include "main.h"

void GSM_SendCommand(char *cmd);
void GSM_SendSMS(char *number,char *msg);

#endif

#endif /* INC_GSM_H_ */
