//**********************************************************
// File: yaw.h
//
// Authors: Freddie Pankhurst   (fpa34)
//          Daniel McGregor     (dmc270)
//
// Handles reading and calculating yaw and determining
// the rotation of the helicopter
//
//**********************************************************

#ifndef YAW_H_
#define YAW_H_

#include <stdint.h>
#include <stdbool.h>

//**********************************************************
// Constant variables
//**********************************************************

// Bounding constants (also for some calcs)
#define N_SLOTS 448
#define FULL_ROTATION 360
#define MAX_YAW 180
#define MIN_YAW -180

// Subdegree calculation
#define DECIMAL_PLACES 10

// Amount to inc/decrement desired yaw
#define YAW_INCREMENT 15

// Struct containing the degree and sub degree for the rotation of the heli
typedef struct
{
    int16_t degree;
    int8_t sub_degree;
} YawPosition;

// Quadrature decoding states
typedef enum {
    INVALID = -1,
    LO_LO,
    HI_LO,
    LO_HI,
    HI_HI
} QDStates;

//**********************************************************
// initYaw: initialize the YawRefIntHandler and
// YawIntHandler pin interupts
//**********************************************************
void initYaw(void);

// **********************************************************
// updateYaw: updates the yaw_pos values by converting the
// yaw into degrees
// **********************************************************
void updateYaw(void);

//**********************************************************
// boundYawDeg: bounds an int16_t in degrees to between
// the MAX and MIN rotation (-179.9->180)
//**********************************************************
int16_t boundYawDeg(int16_t boundingVar);

//**********************************************************
// getYaw: Returns the stuct containing the current rotation
// in degrees (including subdegree)
//**********************************************************
YawPosition getYaw(void);

//**********************************************************
// incrementYaw: Increases the desired yaw by YAW_INCREMENT
// and then rebounds it back to +-180
//**********************************************************
void incrementYaw(void);

//**********************************************************
// decrementYaw: Decreases the desired yaw by YAW_INCREMENT
// and then rebounds it back to +-180
//**********************************************************
void decrementYaw(void);

//**********************************************************
// getDesiredYaw: Returns the desired yaw value
//**********************************************************
int16_t getDesiredYaw(void);

//**********************************************************
// getYawError: Returns current yaw error, between the
// currrent position and the desired position
//**********************************************************
int16_t getYawError(void);

//**********************************************************
// getYawRef: Returns the falg signaling if the yawRef
// interupt has been triggered
//**********************************************************
bool getYawRef(void);

//**********************************************************
// resetYawRef: Reset the yawRef flag back to false, so
// taking off calcibration can be untertaken multiple times
//**********************************************************
void resetYawRef(void);

//**********************************************************
// resetYaw: Resets the yaw, same as zeroing all rotation
// about where you call resetYaw()
//**********************************************************
void resetYaw(void);

//**********************************************************
// resetDesiredYaw: Resets the desired yaw to 0
//**********************************************************
void resetDesiredYaw(void);

#endif // YAW_H_
