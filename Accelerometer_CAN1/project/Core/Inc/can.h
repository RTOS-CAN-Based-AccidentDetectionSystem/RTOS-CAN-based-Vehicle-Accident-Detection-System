/*
 * can.h
 *
 *  Created on: 14-Jun-2026
 *      Author: sunbeam
 */

#ifndef _CAN_H_
#define _CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern CAN_HandleTypeDef hcan1;

static void MX_CAN1_Init(void);

#ifdef __cplusplus
}
#endif
#endif /* INC_CAN_H_ */
