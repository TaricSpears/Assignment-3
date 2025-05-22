#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "config.h"
#include "devices/Button.h"
#include "devices/ButtonImpl.h"
#include "devices/Pot.h"
#include "devices/servo_motor.h"
#include "devices/servo_motor_impl.h"
#include "kernel/MsgService.h"

enum Mode { Automatic, Manual } mode;

ServoMotor* servoMotor = new ServoMotorImpl(WINDOW_MOTOR_PIN);
Button* button = new ButtonImpl(BUTTON_PIN);
Potentiometer* potentiometer = new Potentiometer(POT_PIN);
LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(0x27, 20, 4);
float opening = 0;
float temperature = 0;
SetmodePattern* setmodePattern = new SetmodePattern();
SetopeningPattern* setopeningPattern = new SetopeningPattern();
TemperaturePattern* temperaturePattern = new TemperaturePattern();

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
                MsgService.sendMsg("SETMODE:MANUAL");
                mode = Manual;
                break;
            case Manual:
                MsgService.sendMsg("SETMODE:AUTOMATIC");
                mode = Automatic;
                break;
        }
    }

    while (MsgService.isMsgAvailable()) {
        Msg* msg = MsgService.receiveMsg(*setmodePattern);
        String content = msg->getContent();
        if (setmodePattern->match(*msg)) {
            if (content == "SETMODE:MANUAL") {
                mode = Manual;
            } else if (content == "SETMODE:AUTOMATIC") {
                mode = Automatic;
            }
        } else if (setopeningPattern->match(*msg)) {
            if (content.startsWith("SETOPENING:")) {
                String openingStr = content.substring(11);
                opening = openingStr.toFloat();
                int angle = (int)(opening * 90);
                servoMotor->setPosition(angle);
            }
        } else if (temperaturePattern->match(*msg)) {
            if (content.startsWith("TEMPERATURE:")) {
                String temperatureStr = content.substring(12);
                temperature = temperatureStr.toFloat();
            }
        }
    }

    if (mode == Manual) {
        potentiometer->sync();
        float potValue = potentiometer->getValue();
        int angle = (int)(potValue * 90);
        servoMotor->setPosition(angle);
        opening = potValue * 100;
        MsgService.sendMsg("SETOPENING:" + String(opening));
    }

    if (mode == Manual) {
        displayMessage(("Opening: " + String(opening) + "%").c_str(), ("Manual, Temp: " + String(temperature) + "C").c_str());
    } else {
        displayMessage(("Opening: " + String(opening) + "%").c_str(), "Automatic");
    }
    delay(50);
}