/*
 * ping.c
 *
 *  Created on: Oct 22, 2022
 *      Author: jdicklin
 */

#include "ping.h"
#include "timer.h"
#include <math.h>

volatile unsigned int start= 0, end = 0;
volatile enum {LOW, HIGH, DONE} state;

volatile int* OverFlow_ptr;
volatile unsigned int* clockCycle_ptr;



void ping_init(void) {
        SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R | 0b000010;   //Setting up the GPIO
        SYSCTL_RCGCTIMER_R |= 0b00001000;

        GPIO_PORTB_DEN_R |= 0b00001000;                     // Setting up digital and output mode of pin 3
        GPIO_PORTB_DIR_R |= 0b00001000;
        GPIO_PORTB_PCTL_R |= 0x00007000;                    // Use timer

        TIMER3_CTL_R &= ~0x100;
        timer_waitMicros(5);

        TIMER3_CFG_R = 0x00000004;                          //Configure timmer 3b
        TIMER3_TBMR_R |= 0b10111;
        TIMER3_CTL_R |= 0xC00;
        TIMER3_TBPR_R = 0xFF;
        TIMER3_TBILR_R = 0xFFFFFF;
        TIMER3_ICR_R |= 0b10000000000;
        TIMER3_IMR_R |= 0b10000000000;

        NVIC_PRI9_R |= 0x000020;                            //Configure NVIC
        NVIC_EN1_R |= 0x10;

            IntRegister(INT_TIMER3B, ping_interrupt_handler);
            TIMER3_CTL_R |= 0x100;
            IntMasterEnable();
}



double ping_read(void) {
    ping_send();
        int time = 0;
        while(state != DONE){}
        if (start > end){            //Determines distance using the systems clock cycles.
            *OverFlow_ptr += 1;
            end += 16777215;
            }
                time = end - start;
                     *clockCycle_ptr = time;
        double value = ((time / 16000000.0) * 340/2) * 100;              //Clock cycles to distance conversion
   return value;
}



void ping_send(void) {
        GPIO_PORTB_AFSEL_R &= ~0b00001000;
        GPIO_PORTB_DIR_R |= 0x08;
        GPIO_PORTB_DATA_R &= ~0x08;     //Send pulse
        timer_waitMicros(2);
        GPIO_PORTB_DATA_R |= 0x08;
        timer_waitMicros(5);
        GPIO_PORTB_DATA_R &= ~0x08;
        GPIO_PORTB_AFSEL_R |= 0x08;     //Enable AFSEL
        GPIO_PORTB_DIR_R &= ~0x08;
        state = LOW;
}



void ping_interrupt_handler(void){
    if (TIMER3_MIS_R & 0x400){
            if (state == LOW){
                state = HIGH;
                start = TIMER3_TBR_R;
                }

            else if (state == HIGH){
                state = DONE;
                timer_waitMicros(10);
                end = TIMER3_TBR_R;
                }
            TIMER3_ICR_R |= 0x400;      //clear interrupt
        }
}



void add_ptrs(int *temp_ptr, unsigned int *temp2_ptr){         //Initializes pointers
    OverFlow_ptr = temp_ptr;
    clockCycle_ptr = temp2_ptr;
}
