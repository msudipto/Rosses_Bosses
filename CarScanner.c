/*
 * CarScanner.c
 *
 *  Created on: Dec 6, 2022
 *      Author: jdicklin
 */

#include "CarScanner.h"
#include "adc.h"
#include "ping.h"
#include "timer.h"
#include "servo.h"
#include <math.h>

int Scan_Objects(){
    double PING[20];        //Declare needed items
    double PINGtemp[100];

    int start_degrees[0];   //start degree of detected objects
    int end_degrees[15];    //end degree of detected objects
    int sdi = 0;            //sdi = start dergee index
    int edi = 0;            //edi = end degree index
    int pui = 0;            //pui = ping under index
    int pti = 0;            //pti = ping temp index
    int start = 1;
    int num_objects = 0;    //number of items detected index

        ping_init();        //Initalize needed items
        servo_init();
        init_ADC();

        servo_move(0);      //Place servo at inital position
        current_degree = 0;
        timer_waitMillis(1000);



        while (current_degree <= 15){
            servo_move(current_degree);
            double sensor = 0.0;
            double toAverage[16];   //store 16 samples for averaging the readings
            int i = 0;
            int ADC_result = adc_read(toAverage); //average the IR sensor values
            double PING_result = ping_read();

                for (i = 0; i < 16; i++){
                    get_sensor_reading(&sensor);
                    toAverage[i] = sensor;
                }


                    timer_waitMillis(70);


                    if (ADC_result < 68 && PING_result < 68){    //if the object is under 68 centimeters, it will be counted as a detected object
                        PINGtemp[pti] = PING_result;
                        pti++;

                    if (start){
                        start_degrees[sdi] = current_degree; //save the starting degree of the object
                        sdi++;
                    }

                        start = 0;
                    }



            if (((PING_result > 68 || ADC_result > 68)||(PING_result < 68 && ADC_result < 68 && current_degree == 15))&& start == 0){ // setting the limits of what is counted as an object
                int i = 0;
                double totalP = 0;
                    i = 0;

                    while (PINGtemp[i] > 0 && PINGtemp[i] < 68){
                        totalP += PINGtemp[i];
                        i++;
                    }

                    totalP = totalP / (double) i; //find the average ping value
                    PING[pui] = totalP;
                    pui++;

                        end_degrees[edi] = current_degree - 5; //Our bot needed a buffer added to get the degree range we wanted
                        edi++;
                        num_objects++;
                        pti = 0;
                        start = 1;
            }
            current_degree += 1;
        }
     return num_objects;
}
