/*
 * movement.h
 *
 *  Created on: Sep 6, 2022
 *      Author: mdrief
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"
/**
 * @brief Move the roomba the specified number of millimeters.
 *
 */
char move_forward(oi_t *sensor, int millimeters);

/**
 * @brief Have the roomba turn the specified number of degrees clockwise
 *
 */
char turn_clockwise(oi_t *sensor, int degrees);

#endif /* MOVEMENT_H_ */
