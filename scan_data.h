/*
 * scan_data.h
 *
 *  Created on: Oct 22, 2022
 *      Author: jdicklin
 */

#include <stdio.h>

#ifndef SCAN_DATA_H_
#define SCAN_DATA_H_
#define MAX_DEGREES 180
#define FILE_LINE_PARAMS 3

struct reading{
    float ir_distance;  //Holds Ping + IR data
    float sonar_distance;
};

void get_reading(char *file_name, struct reading *reading_array);

#endif /* SCAN_DATA_H_ */
