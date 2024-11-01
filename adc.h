/*
 * adc.h
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

#ifndef ADC_H_
#define ADC_H_

void get_sensor_reading(double *sensor);
void init_ADC();
int adc_read(double average[]);

#endif /* ADC_H_ */
