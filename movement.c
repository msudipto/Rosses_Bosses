/*
 * movement.c
 *
 *  Created on: Sep 6, 2022
 *  Last Updated: Dec 1, 2022
 *      Author: Matthew Rief
 *
 *      This movement file was tuned to work for Cybot 3
 */

#include "movement.h"
#include "lcd.h"
int speed = 80;
double rightMotorSpeedMultiplier = 0.93; //The right motor seems a little overpowered and the bot will pull to the left in testing so we just make it slower to drive straight.
double LeftMotorSpeedMultiplier = 1.0;

char move_forward(oi_t *sensor, int millimeters)
{
    char status = 'g'; // g:movement complete without issue , c:any cliff sensor was triggered, b:any bump sensor was triggered
    oi_t *sensor_data = sensor;
    oi_init(sensor_data);
    double TotalDisplacement = 0;
    if (millimeters < 0) //move backwards
    {
        oi_setWheels(-speed * rightMotorSpeedMultiplier,
                     -speed * LeftMotorSpeedMultiplier);
        while (TotalDisplacement > millimeters)
        {
            oi_update(sensor_data);
            TotalDisplacement += sensor_data->distance;
        }
    }

    else
    { // move forward
        oi_setWheels(speed * rightMotorSpeedMultiplier,
                     speed * LeftMotorSpeedMultiplier);
        while (TotalDisplacement < millimeters) //While we have not gone the full distance we wanted to move...
        {
            oi_update(sensor_data); //Keep asking the Roomba for updated sensor data ...
            TotalDisplacement += sensor_data->distance; //Recalculate the total distance we have moved... (sensordata.distance holds the number of millimeters the Roomba thinks it has traveled since the last update)
            if (sensor_data->cliffFrontRight || sensor_data->cliffFrontLeft) //If you are about to run off the end of some edge, then stop!
            {
                status = 'c'; // c means cliff detected
                lcd_printf("Cliff!");
                break; //Break the loop to stop movement
            }
            if (sensor_data->bumpRight || sensor_data->bumpLeft)// If you have bumped into something then stop!
            {
                status = 'b'; // c means a cliff detected
                lcd_printf("Wall!");
                break;
            }
        }
    }
    oi_setWheels(0, 0); // stop
    oi_free(sensor_data);
    return status;
}

char turn_clockwise(oi_t *sensor, int degrees)
{
    char status = 'g';
    oi_t *sensor_data = sensor;
    int speed = 80; // 500 is full speed
    oi_init(sensor_data);
    degrees = degrees * (-1); // The roomba's internal sensor counts turning counter clockwise as the positive direction for positve degrees, so we just flip the number negative to make it work for turning clockwise
    double sum = 0;
    if (degrees < 0)
    {
        oi_setWheels(-speed * rightMotorSpeedMultiplier,
                     speed * LeftMotorSpeedMultiplier);
        while (sum > degrees)
        {
            oi_update(sensor_data);
            sum += sensor_data->angle;
        }
    } //turn CCW; full speed
    else
    {
        oi_setWheels(speed * rightMotorSpeedMultiplier,
                     -speed * LeftMotorSpeedMultiplier);
        while (sum < degrees)
        {
            oi_update(sensor_data);
            sum += sensor_data->angle;
        }
    } // Turn CW; full speed

    oi_setWheels(0, 0); // stop
    oi_free(sensor_data);
    return status;
}

