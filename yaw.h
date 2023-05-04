/*
 * Yaw.h
 *
 *  Created on: 3/05/2023
 *      Author: dmc270
 */

#ifndef YAW_H_
#define YAW_H_

#include <stdint.h>
#include <stdbool.h>

#define N_SLOTS 448
#define FULL_ROTATION 360
#define MAX_YAW 180
#define MIN_YAW -180
#define DECIMAL_PLACES 10

typedef struct
{
    int32_t degree;
    int8_t sub_degree;
} YawPosition;

// quadrature decoding states
enum {
    INVALID = -1,
    LO_LO,
    HI_LO,
    LO_HI,
    HI_HI
} QDStates;

void initYaw(void);

void YawIntHandler(void);

void updateYaw(void);

YawPosition getYaw(void);

#endif /* YAW_H_ */
