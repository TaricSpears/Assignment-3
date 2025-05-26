// File: state_machine.h
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

enum State
{
    NORMAL,
    HOT,
    TOOHOT,
    ALARM
};

void stateInit();
void updateState(float temperature);
const char *getStateString();
void updateLEDs();

#endif // STATE_MACHINE_H