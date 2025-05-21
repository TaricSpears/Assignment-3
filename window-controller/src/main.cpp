#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "config.h"
#include "devices/Button.h"
#include "devices/ButtonImpl.h"
#include "devices/Pot.h"
#include "devices/servo_motor.h"
#include "devices/servo_motor_impl.h"

enum Mode { Automatic, Manual } mode;
enum WindowState { Open, Closed } windowState;

ServoMotor* servoMotor = new ServoMotorImpl(WINDOW_MOTOR_PIN);
Button* button = new ButtonImpl(BUTTON_PIN);
Potentiometer* potentiometer = new Potentiometer(POT_PIN);
LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(0x27, 20, 4);
float opening = 0;

void displayMessage(const char* message) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(message);
}

void displayMessage(const char* line1, const char* line2) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(line1);
    lcd->setCursor(0, 1);
    lcd->print(line2);
}

float getTemperature() {
    // Simulate temperature reading
    return random(20, 30);
}

void setup() {
    mode = Automatic;
    servoMotor->on();
    servoMotor->setPosition(0);
    lcd->init();
    lcd->backlight();
}

void loop() {
    button->sync();
    if (button->isClicked()) {
        switch (mode) {
            case Automatic:
                mode = Manual;
                break;
            case Manual:
                mode = Automatic;
                break;
        }
    }

    if (mode == Manual) {
        potentiometer->sync();
        float potValue = potentiometer->getValue();
        int angle = (int)(potValue * 90);
        servoMotor->setPosition(angle);
        opening = potValue * 100;
    }

    if (mode == Manual) {
        displayMessage(("Opening: " + String(opening) + "%").c_str(),
                       ("Manual, Temp: " + String(getTemperature()) + "C").c_str());
    } else {
        displayMessage(("Opening: " + String(opening) + "%").c_str(), "Automatic");
    }
    delay(100);
}