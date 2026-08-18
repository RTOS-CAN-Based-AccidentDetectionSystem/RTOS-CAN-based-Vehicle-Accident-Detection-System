#ifndef GSM_H
#define GSM_H

#include "main.h"

void GSM_Init();
void GSM_SendCommand(char *cmd);
void GSM_SendSMS(char *number, char *message);

#endif
