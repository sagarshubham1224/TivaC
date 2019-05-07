/*
 * BRUSHLESS_MOTOR.h
 *
 *  Created on: 06-Apr-2019
 *      Author: sagarshubham
 */

#ifndef BRUSHLESS_MOTOR_H_
#define BRUSHLESS_MOTOR_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "PWM.h"
#include "GPIO.h"
#include "TIMER.h"

#define WHEEL_RADIUS_CM                 2.115f
#define MOTOR_PWM_PRESCALAR             PWM_PRESCALAR_64
#define MOTOR_PWM_FREQUENCY             (490/2)
#define MOTOR_MIN_SPEED                 (-100)
#define MOTOR_MAX_SPEED                 (100)

typedef enum DC_MOTOR_STATE {
    MOTOR_ON    =   true ,
    MOTOR_OFF   =   false
}DC_MOTOR_STATE;

typedef enum DC_MOTOR_DIRECTION {
    FORWARD = 1 ,
    BACKWARD = -1 ,
    STALLING = 0
}DC_MOTOR_DIRECTION;


typedef struct ENCODER {
    GPIO digitalPin ;
    GPIO interruptPin ;
}ENCODER ;

typedef enum ENCODER_INTERRUPT_TYPE {
    RISING_INT      = GPIO_RISING_EDGE ,
    FALLING_INT     = GPIO_FALLING_EDGE ,
    BOTH_INT        = GPIO_BOTH_EDGES,
    NO_INT          = 0
}ENCODER_INTERRUPT_TYPE;
#define NO_MOTOR_INTERRUPT              NULL


typedef struct DC_MOTOR {
    PWMPin                  speedControlPin ;
    GPIO                    directionControlPin ;
    DC_MOTOR_STATE          currentMotorState ;
    ENCODER                 motorEncoder ;
    DC_MOTOR_DIRECTION      motorCurrentDirection ;
    uint64_t                lastRiseTime ;
    uint64_t                currentRiseTime ;
    int32_t                 pulseCount ;
    int32_t                 revPulseCount ;
    int32_t                 minPulseCount ;
    int32_t                 maxPulseCount ;
    int8_t                  motorDesiredSpeed ;
    int32_t                 motorDesiredDistanceInPulses ;
    float                   Kp ;
} DC_MOTOR ;




/*
 * Function to initialize DC Brushless Motor.
 * Arguments:
 *  DC_MOTOR*       MotorDevicePointer                  :: Pointer to DC_MOTOR struct.
 *  GPIO_PIN_CODE   SpeedPinCode                        :: GPIO Pin Code for PWM Pin for Speed Control.
 *  GPIO_PIN_CODE   DirectionPinCode                    :: GPIO Pin Code for GPIO Pin for Direction Control.
 *  GPIO_PIN_STATE  forwardDirectionPinControlState     :: HIGH or LOW depending upon connections.
 *  GPIO_PIN_STATE  backwardDirectionPinControlState    :: HIGH or LOW depending upon connections.
 * Returns:
 *  none.
 */
void initializeDCMotor(DC_MOTOR*        MotorDevicePointer,
                       GPIO_PIN_CODE    SpeedPinCode,
                       GPIO_PIN_CODE    DirectionPinCode,
                       GPIO_PIN_CODE    encoderDigitalPin,
                       GPIO_PIN_CODE    encoderInterruptPin,
                       ENCODER_INTERRUPT_TYPE   encoderInterruptType,
                       void (*encoderInterruptHandler)(void)) ;

/*
 * Function to set Motor on or off.
 * Arguments:
 *  DC_MOTOR*      MotorDevicePointer           :: Pointer to DC_MOTOR struct.
 *  DC_MOTOR_STATE MotorState                   :: State to be set.
 * Returns:
 *  none.
 */
void setMotorState(DC_MOTOR*        MotorDevicePointer,
                   DC_MOTOR_STATE   MotorState) ;
/*
 * Function to get current Motor state.
 * Arguments:
 *  DC_MOTOR*      MotorDevicePointer           :: Pointer to DC_MOTOR struct.
 * Returns:
 *  DC_MOTOR_STATE MotorState                   :: Current state. True = On, False = Off.
 */
DC_MOTOR_STATE getMotorState(DC_MOTOR*        MotorDevicePointer) ;

/*
 * Function to set Motor speed.
 * Arguments:
 *  DC_MOTOR*       MotorDevicePointer          :: Pointer to DC_MOTOR struct.
 *  int8_t          MotorSpeed                  :: Speed of the Motor.
 * Returns:
 *  none.
 */
void setMotorSpeed(DC_MOTOR*        MotorDevicePointer,
                   int8_t           MotorSpeed) ;

void setMotorDistance(DC_MOTOR* MotorDevicePointer, float distanceCM) ;
#endif /* BRUSHLESS_MOTOR_H_ */
