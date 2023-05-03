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

typedef struct
{
    int32_t degree;
    int8_t sub_degree;
} YawPosition;

static int32_t yaw;
static YawPosition yaw_pos;

void initYaw(void);

void YawIntHandler(void);

void updateYaw(void);

YawPosition getYaw(void);

#endif /* YAW_H_ */
