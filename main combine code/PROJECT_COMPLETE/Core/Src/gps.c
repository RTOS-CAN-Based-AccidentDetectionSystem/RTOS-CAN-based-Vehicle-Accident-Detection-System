/*
 * gps.c
 *
 *  Created on: 30-Jul-2026
 *      Author: sunbeam
 */
#include "gps.h"
#include "string.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;

char latitude[12] = "0.000000";
char longitude[12] = "0.000000";

char gpsBuffer[100];
uint8_t gpsIndex = 0;
uint8_t gpsRx;

void GPS_Read(void)
{
    gpsIndex = 0;

    while(1)
    {
        HAL_UART_Receive(&huart1, &gpsRx, 1, HAL_MAX_DELAY);

        if(gpsRx == '\n')
        {
            gpsBuffer[gpsIndex] = '\0';
            GPS_Parse();
            break;
        }

        gpsBuffer[gpsIndex++] = gpsRx;

        if(gpsIndex >= sizeof(gpsBuffer)-1)
            gpsIndex = 0;
    }
}

void GPS_Parse(void)
{
    char *token;

    if(strncmp(gpsBuffer,"$GPGGA",6)==0)
    {
        token = strtok(gpsBuffer,",");

        int field = 0;

        while(token != NULL)
        {
            field++;

            if(field == 3)
                strcpy(latitude,token);

            if(field == 5)
            {
                strcpy(longitude,token);
                break;
            }

            token = strtok(NULL,",");
        }
    }
}
