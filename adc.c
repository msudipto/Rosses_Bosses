/*
 * adc.c
 *
 *  Created on: Oct 22, 2022
 *      Author: jdicklin
 */

#include "adc.h"
#include <math.h>



void init_ADC(){
     SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R | 0b000010; //Setup for GPIO ADC on Port B
     SYSCTL_RCGCADC_R |= 0x1;
     GPIO_PORTB_AFSEL_R |= 0b00010000;

     GPIO_PORTB_DIR_R &= 0b11101111; //Setup pins for input + analog mode
     GPIO_PORTB_DEN_R &= 0b11101111;
     GPIO_PORTB_AMSEL_R |= 0b00010000;

     GPIO_PORTB_ADCCTL_R = 0x0; //Setup for default sample trigger source

     ADC0_ACTSS_R &= 0xFFFE; //Disable ADC0 SS0

     ADC0_EMUX_R &= 0xFFF0; // Set ADC SS0 functionality
     ADC0_SSMUX0_R |= 0x0000000A;

     ADC0_SSCTL0_R = 0x00000000; //Setup end sample of sequence
     ADC0_SSCTL0_R |= 0x00000006;

     ADC0_ACTSS_R |= 0x1; //Set up ADC interrupts
}



void get_sensor_reading(double *sensor){
    ADC0_PSSI_R = 0x1; // Initiate SS0 conversion sequence

    while((ADC0_RIS_R & 0x1) == 0){} // Wait for SS0 ADC conversions

    ADC0_ISC_R = 0x01; // Clear raw interrupt status flag for SS0

    *sensor = ADC0_SSFIFO0_R; // Get SS0 FIFO converted results
}



int adc_read(double average[]){
    double sum = 0;   //Store value sum

        int i, result;
            for(i = 0; i < 16; i++)    //Simple math to average out the values (add all 16 together, then divide by 16)
                sum+= average[i];
                    sum = sum/16.0;


                    double x = pow(sum, -1.98450954912);
                    double y = 60816502.5227*x;
                    y = round(y);           //Turn the value to a whole integer
                    result = (int) y;
    return result;
}
