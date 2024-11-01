/*
 * servo.c
 *
 *  Created on: Oct 22, 2022
 *      Author: jdicklin
 */

#include "servo.h"

volatile double current_degree = 0;
volatile int clockwise = 0;
volatile unsigned int matcher;

void servo_init(void) {
    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R | 0b000010;     //Setup timer 1 clock
    SYSCTL_RCGCTIMER_R |= 0b00000010;

    GPIO_portb_init();

    TIMER1_CTL_R &= ~0x100;                               //Setup timer 1B
    timer_waitMicros(5);
    TIMER1_CFG_R = 0x4;
    TIMER1_TBMR_R |= 0b01010;
    TIMER1_TBMR_R &= 0b11011;

    unsigned int pwm_period = 320000;
    TIMER1_TBILR_R  = pwm_period & 0x00FFFF;
    TIMER1_TBPR_R  = pwm_period >> 16;
    TIMER1_CTL_R |= 0b100000000;
}



void GPIO_portb_init(void){
    GPIO_PORTB_AFSEL_R |= 0b00100000;    //Setup GPIO port B
    GPIO_PORTB_PCTL_R |= 0x00700000;
    GPIO_PORTB_DEN_R |= 0b00100000;
    GPIO_PORTB_DIR_R |= 0b00100000;
}



void servo_move(double degrees) {
    matcher = 312155 - 152.77 * degrees;
    TIMER1_TBMATCHR_R = matcher & 0x00FFFF;
    TIMER1_TBPMR_R = matcher >> 16;
    unsigned int duty_cycle = (TIMER1_TBILR_R - TIMER1_TBMATCHR_R)/TIMER1_TBILR_R; // high/period
}



double servo_get_degree(void){
    return current_degree;                     //Sends back the current degree
}
