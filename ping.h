/*
 * ping.h
 *
 *  Created on: Oct 22, 2022
 *      Author: jdicklin
 */

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

#ifndef PING_H_
#define PING_H_

extern volatile unsigned int start, end;

void ping_init(void);
double ping_read(void);
void ping_send(void);
void ping_interrupt_handler(void);
void add_ptrs(int *temp_ptr, unsigned int *temp2_ptr);

#endif /* PING_H_ */
