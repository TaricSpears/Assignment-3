#ifndef __SERVO_MOTOR_IMPL__
#define __SERVO_MOTOR_IMPL__

#include <Arduino.h>

#include "ServoTimer2.h"
#include "servo_motor.h"

class ServoMotorImpl : public ServoMotor {
   public:
    ServoMotorImpl(int pin);

    void on();
    void setPosition(int angle);
    void off();

   private:
    int pin;
    ServoTimer2 motor;
};

#endif
