/*
 * Final_main.c
 *
 *  Created on: Dec 6, 2022
 *      Author: mdrief
 */

#define _RESET 0
#define _Testing 0
#define _PART1 1

#include "timer.h"
#include "lcd.h"
// Provided libraries that allow use of timing fuctions and functions for printing to the Cybot LCD character display

#include "uart_extra_help.h"
// Custom written library that sets up UART serial communication hardware of the microcontroler.

#include "CarScanner.h"
// Car Scanner contains a custom helper function that will use the sonar, and IR sensors on the Cybot to detect nearby objects.

#include "open_interface.h"
/* The open interface contains a set of functions and data structures provided by the Roomba the
 * Cybot is built on. Through this library we can control motor speeds and read sensor data from sensors built into the Roomba
 */

#include "movement.h"
// Another custom written library for controlling the Roombas motors to move the bot around

#include <math.h>

extern volatile char received_char; // Your UART interupt code can place read data here
extern volatile int receive_event;  // Your UART interupt can update this flag to indicate that it has placed new data

void main()
{
#if _RESET
//    resetSimulationBoard();
#else

    timer_init(); // Initialize the Microcontroller's timers
    lcd_init(); // Initialize communications to the Cybot LCD display
    uart_init(115200); // Set up uart serial communications with the desktop computer (the desktop will host the user interface and communicate over putty)
    char string[100]; // This string is a buffer that we can write messages to using sprintf, and then we can use uart_Sendstr(string) to send the message over uart back to the desktop

#if _Testing
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    while(1){
        oi_update(sensor_data);
        sensor_data->
        timer_waitMillis(1000);
    }
#endif

    int parkingSpots[14] = { 1 }; //This array represents the parking spots, we denote empty/available parking spots as zero, full spots contain 1 or greater (the greater would represent something like double parking two motorcycles in one spot)
    int parkingIndex = 1;

#if _PART1
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    uart_interrupt_init();
    char status = ' '; // the status will be changed by various functions such as moving forward can change the status to 'c' for cliff detected!
    while (1) // This is the main program loop, it will check if the user has entered a character over the putty uart interface denoted by setting the "receive_event" flag to a 1. From there the program will react to different commands depending on the recived character.
    {
        if (receive_event)
        {
            receive_event = 0; // Clear the receive event

            //The tiles we want to move along are approximatly 600mm long, there is some fudge factor in most of the movement as motors and servos on different bots behave differently. Our movement numbers have been calibrated to cybot 3
            if (received_char == 'w') //move forward
            {
                lcd_printf("Moving Forward");
                sprintf(string, "Moving Forward: ");
                uart_sendStr(string);
                status = move_forward(sensor_data, 603); //Move about 600 mm forward
                sprintf(string, "%c\n", status);
                uart_sendStr(string);
                received_char = 0;
            }
            else if (received_char == 'a') //Turn 90 degrees left
            {
                lcd_printf("Turning Left");
                sprintf(string, "Turning Left: ");
                uart_sendStr(string);
                status = turn_clockwise(sensor_data, -83);
                sprintf(string, "%c\n", status);
                uart_sendStr(string);
                received_char = 0;
            }
            else if (received_char == 's') //Move in reverse
            {
                lcd_printf("Moving Backward");
                sprintf(string, "Moving Backward: ");
                uart_sendStr(string);
                status = move_forward(sensor_data, -603);
                sprintf(string, "%c\n", status);
                uart_sendStr(string);
                received_char = 0;
            }
            else if (received_char == 32) //Move in reverse just a few millimeters , 32 is the space bar ascii code
            {
                lcd_printf("Moving Backward");
                sprintf(string, "Moving Backward: ");
                uart_sendStr(string);
                status = move_forward(sensor_data, -30);
                sprintf(string, "%c\n", status);
                uart_sendStr(string);
                received_char = 0;
            }
            else if (received_char == 'd') //Turn right 90 degrees
            {
                lcd_printf("Turning Right");
                sprintf(string, "Turning Right: ");
                uart_sendStr(string);
                status = turn_clockwise(sensor_data, 84);
                sprintf(string, "%c\n", status);
                uart_sendStr(string);
                received_char = 0;
            }
            else if (received_char == 't') //Turn right 90 degrees
            {
                oi_free(sensor_data);
            }
            else if (received_char == '3') // Scan a region to the right of the bot and look for any objects using the ir sensor. If an object is found mark that parking space as full
            {

                lcd_printf("Scanning Right Side");
                sprintf(string, "Scanning Right Side: \n");
                uart_sendStr(string);

                parkingSpots[parkingIndex] = Scan_Objects(); //Scan objects will return the number of objects found in a 0 to 30 degree sweep to the right of the cybot, we save the result for later reporting

                if (parkingSpots[parkingIndex] > 0)
                {
                    lcd_printf("Spot: %d Full!", parkingIndex);
                    sprintf(string, "Spot: %d Full!\n", parkingIndex);
                    uart_sendStr(string);
                }
                else
                {
                    lcd_printf("Spot: %d Available!", parkingIndex);
                    sprintf(string, "Spot: %d Available!\n", parkingIndex);
                    uart_sendStr(string);
                }
                parkingIndex++; // Automatically incriment what parking spot # we are looking at as we have just obatained a result for the last one.

            }
            else if (received_char == '9') // Mark the next spot full, for use when we hit a "pothole" or other obstacle that makes the spot unreachable
            {

                lcd_printf("Scanning Right Side");
                sprintf(string, "Scanning Right Side: \n");
                uart_sendStr(string);

                parkingSpots[parkingIndex] = 1;

                lcd_printf("Spot: %d Full!", parkingIndex);
                sprintf(string, "Spot: %d Full!\n", parkingIndex);
                uart_sendStr(string);

                parkingIndex++; // move the pointer to the next parking spot in the list

            }
            else if (received_char == 'r') // Ask the Cybot for a parking report, while there are 0-13 spots in the parking garage, the bot will only report on the ones it has scanned so far (I.E. up to the last value of "parking index")
            {

                sprintf(string, "Full: ");
                uart_sendStr(string);
                int i = 0;
                for (i; i < parkingIndex; i++) // List out the full spots
                {
                    if (parkingSpots[i] > 0) //The spot was found to have one or more objects parked there
                    {
                        sprintf(string, "%d,", i);
                        uart_sendStr(string);
                    }
                }
                i = 0;
                sprintf(string, "\nEmpty: ");
                uart_sendStr(string);
                for (i; i < parkingIndex; i++) // List out the empty spots
                {
                    if (parkingSpots[i] < 1) // There were no objects in the spot when we scanned
                    {
                        sprintf(string, "%d,", i);
                        uart_sendStr(string);
                    }
                }
                sprintf(string, "\n");
                uart_sendStr(string);

            }

            else if (received_char == 'f') // f will have the bot send a response for a quick test if communication is working
            {
                sprintf(string, "Hello!\n");
                uart_sendStr(string);
            }
        }
    }
#endif

#endif
}

