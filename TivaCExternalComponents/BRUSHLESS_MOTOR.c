/*
 * BRUSHLESS_MOTOR.c
 *
 *  Created on: 06-Apr-2019
 *      Author: sagarshubham
 */

#include "BRUSHLESS_MOTOR.h"



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
                       void (*encoderInterruptHandler)(void))
{
    //    MotorDevicePointer->motorDirectionControl.forwardDirectionControl = LOW ;
    //    MotorDevicePointer->motorDirectionControl.backwardDirectionControl = HIGH ;
    initPWM(&MotorDevicePointer->speedControlPin,
            SpeedPinCode,
            MOTOR_PWM_PRESCALAR,
            MOTOR_PWM_FREQUENCY ) ;
    initGPIO(&MotorDevicePointer->directionControlPin, DirectionPinCode, OUTPUT) ;

    if(encoderInterruptHandler != NULL)
    {
        initGPIO(&MotorDevicePointer->motorEncoder.digitalPin, encoderDigitalPin, INPUT) ;
        initGPIO(&MotorDevicePointer->motorEncoder.interruptPin,encoderInterruptPin,INPUT) ;
        initGPIOI(&MotorDevicePointer->motorEncoder.interruptPin, encoderInterruptType , encoderInterruptHandler) ;
    }
    MotorDevicePointer->currentMotorState = MOTOR_OFF ;
    MotorDevicePointer->motorCurrentDirection = STALLING ;
    MotorDevicePointer->minPulseCount = -1400 ;
    MotorDevicePointer->maxPulseCount = 1400 ;
    MotorDevicePointer->motorDesiredDistanceInPulses = 0 ;
    MotorDevicePointer->Kp = 5 ;
}


/*
 * Function to set Motor on or off.
 * Arguments:
 *  DC_MOTOR*      MotorDevicePointer           :: Pointer to DC_MOTOR struct.
 *  DC_MOTOR_STATE MotorState                   :: State to be set.
 * Returns:
 *  none.
 */
void setMotorState(DC_MOTOR*        MotorDevicePointer,
                   DC_MOTOR_STATE   MotorState)
{
    if(MotorState ^ MotorDevicePointer->currentMotorState)
    {
        if(MotorState == MOTOR_OFF)
        {
            analogPWMWrite(&MotorDevicePointer->speedControlPin, 0) ;
            digitalWrite(&MotorDevicePointer->directionControlPin, LOW) ;
            MotorDevicePointer->currentMotorState = MOTOR_OFF ;
        }
        else
        {
            MotorDevicePointer->currentMotorState = MOTOR_ON ;
        }
    }
}

/*
 * Function to get current Motor state.
 * Arguments:
 *  DC_MOTOR*      MotorDevicePointer           :: Pointer to DC_MOTOR struct.
 * Returns:
 *  DC_MOTOR_STATE MotorState                   :: Current state. True = On, False = Off.
 */
DC_MOTOR_STATE getMotorState(DC_MOTOR*        MotorDevicePointer)
{
    return MotorDevicePointer->currentMotorState ;
}

/*
 * Function to set Motor speed.
 * Arguments:
 *  DC_MOTOR*       MotorDevicePointer          :: Pointer to DC_MOTOR struct.
 *  int8_t          MotorSpeed                  :: Speed of the Motor.
 * Returns:
 *  none.
 */
void setMotorSpeed(DC_MOTOR*        MotorDevicePointer,
                   int8_t           MotorSpeed)
{
    if(MotorSpeed < MOTOR_MIN_SPEED || MotorSpeed > MOTOR_MAX_SPEED)
    {
        return ;
    }
    else if (MotorDevicePointer->currentMotorState == MOTOR_OFF)
    {
        return ;
    }
    else if(MotorSpeed < 0 )
    {
        //        MotorSpeed = MOTOR_MAX_SPEED + MotorSpeed ;
        digitalWrite(&MotorDevicePointer->directionControlPin,LOW) ;
        analogPWMWrite(&MotorDevicePointer->speedControlPin,
                       MotorSpeed) ;
    }
    else if(MotorSpeed > 0 )
    {
        MotorSpeed = MOTOR_MAX_SPEED - MotorSpeed ;
        digitalWrite(&MotorDevicePointer->directionControlPin,HIGH) ;
        analogPWMWrite(&MotorDevicePointer->speedControlPin,
                       MotorSpeed) ;
    }
    else if(MotorSpeed == 0)
    {
        digitalWrite(&MotorDevicePointer->directionControlPin, LOW) ;
        analogPWMWrite(&MotorDevicePointer->speedControlPin, 0) ;
        MotorDevicePointer->pulseCount = 0 ;
        MotorDevicePointer->motorCurrentDirection = STALLING ;
    }
}


void setMotorDistance(DC_MOTOR* MotorDevicePointer, float distanceCM)
{
    MotorDevicePointer->motorDesiredDistanceInPulses = (int32_t)(distanceCM*MotorDevicePointer->maxPulseCount)/(2*3.14*WHEEL_RADIUS_CM*2) ;
}
