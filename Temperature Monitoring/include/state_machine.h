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
void forceAlarmState(bool alarmOn); // Nuova funzione per allarme esterno
bool isExternalAlarmActive();       // Funzione per controllare stato allarme esterno

#endif // STATE_MACHINE_H