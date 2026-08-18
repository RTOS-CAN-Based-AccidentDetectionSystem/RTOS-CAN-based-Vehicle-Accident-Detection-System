/*
 * can.c
 *
 *  Created on: 17-Aug-2026
 *      Author: sunbeam
 */


/* =====================================================================
 * can.c - CAN1 peripheral init
 * ---------------------------------------------------------------------
 * Bit timing below targets 500 kbps assuming a 36 MHz APB1 clock
 * (typical for STM32F103 @ 72 MHz SYSCLK, APB1 prescaler /2).
 *
 *   Bit time = 1 Sync + BS1 + BS2 (in time quanta)
 *   Prescaler = 4, BS1 = 9tq, BS2 = 8tq  ->  18 tq/bit
 *   500 kbps = 36 MHz / (4 * 18) = 500,000  ✔
 *
 * If your APB1 clock differs, recompute Prescaler/BS1/BS2 for your
 * target bit rate (use an STM32 bxCAN bit-timing calculator).
 * =====================================================================
 */
#include "can.h"






