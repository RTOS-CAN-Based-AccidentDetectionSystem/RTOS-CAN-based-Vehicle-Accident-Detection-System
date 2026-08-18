/*
 * can.c
 *
 *  Created on: 14-Jun-2026
 *      Author: sunbeam
 */
#include "can.h"

//CAN_HandleTypeDef hcan1;

static void MX_CAN1_Init(void)
{
    hcan1.Instance = CAN1;

    hcan1.Init.Prescaler = 18;
    hcan1.Init.Mode = CAN_MODE_NORMAL;

    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;

    HAL_CAN_Init(&hcan1);
}

