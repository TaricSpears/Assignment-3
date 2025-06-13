#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "Servo.h"
#include "config.h"
#include "devices/Button.h"
#include "devices/ButtonImpl.h"
#include "devices/Pot.h"
#include "kernel/MsgService.h"

enum Mode { Automatic, Manual } mode;

Servo* servoMotor = new Servo();
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
    servoMotor->attach(WINDOW_MOTOR_PIN);
    servoMotor->write(0);
    lcd->init();
    lcd->backlight();
    MsgService.init();
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

    int cnt = 0;
    while (MsgService.isMsgAvailable()) {
        Msg* msg = MsgService.receiveMsg();
        if (msg == NULL) {
            continue;
        }
        String content = msg->getContent();
        cnt++;
        // displayMessage(("received; " + String(cnt) + " messages").c_str());
        // displayMessage(("Received: " + content).c_str());
        // delay(20);
        // Serial.println("Received message: " + content);
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
                int angle = (int)(opening / 100.0 * 90);
                servoMotor->write(angle);
            }
        } else if (temperaturePattern->match(*msg)) {
            if (content.startsWith("TEMPERATURE:")) {
                String temperatureStr = content.substring(12);
                temperature = temperatureStr.toFloat();
            }
        }
        delete msg;
    }

    if (mode == Manual) {
        potentiometer->sync();
        float potValue = potentiometer->getValue();
        int angle = (int)(potValue * 90);
        servoMotor->write(angle);
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