/*
 * gps.h
 *
 *  Created on: 30-Jul-2026
 *      Author: sunbeam
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_

#ifndef GPS_H
#define GPS_H
#include "main.h"
void GPS_Read(void);
void GPS_Parse(void);

extern char latitude[12];
extern char longitude[12];

#endif

#endif /* INC_GPS_H_ */
