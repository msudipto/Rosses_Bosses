/*
 * servo.h
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

#ifndef SERVO_H_
#define SERVO_H_

extern volatile int clockwise;
extern volatile unsigned int matcher;
extern volatile double current_degree;

void servo_init(void);
void GPIO_portb_init(void);
void servo_move(double degrees);
void servo_button_press(int button);
double servo_get_degree(void);
void get_info(char *info);

#endif /* SERVO_H_ */
