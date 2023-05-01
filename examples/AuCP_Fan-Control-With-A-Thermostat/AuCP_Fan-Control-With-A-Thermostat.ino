#include "FiniteState.h"

#define thermostatPin   A0
#define startStatusPin  5
#define stopStatusPin   6


uint8_t statusPins[] = {stopStatusPin, startStatusPin};
const uint8_t numberOfStatus = sizeof(statusPins) / sizeof(uint8_t);

void StartFanProcess(State state);
void StopFanProcess(State state);
bool FanStartPredicate(id_t state);
bool FanStopPredicate(id_t state);

Transition transitions[] = {
  {FanStartPredicate, 0, 1, StartFanProcess}, // State-0 - NextF = 0, NextT = 1
  {FanStopPredicate, 1, 0, StopFanProcess}    // State-1 - NextF = 1, NextT = 0
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition);

FiniteState finiteStateMachine(transitions, numberOfTransitions); // Finite-State Object

const long ThermostatRead();
void FanControl(id_t id);

long temperature;

void setup() {
  for (uint8_t index = 0; index < numberOfStatus; index++) {
    pinMode(statusPins[index], OUTPUT);
    digitalWrite(statusPins[index], LOW);
  }

  finiteStateMachine.begin(0);    // FSM begins with Initial Transition Id 0
}

void loop() {
  finiteStateMachine.execute();   // Execute the FSM
  temperature = ThermostatRead(); // Read temperature
}

bool FanStartPredicate(id_t state) {
  return temperature >= 40;       // Determine Fan Start Action
}

void StartFanProcess(State state) {
  FanControl(state.id);           // Fan control output
}

bool FanStopPredicate(id_t state) {
  return temperature <= 30;       // Determine Fan Stop Action
}

void StopFanProcess(State state) {
  FanControl(state.id);           // Fan control output
}

void FanControl(id_t id) {
  for (uint8_t index = 0; index < numberOfStatus; index++) {
    bool value = index == id;
    digitalWrite(statusPins[index], value);  // Update Status
  }
}

const long ThermostatRead() {
  long value = analogRead(thermostatPin); // Read Pushbutton Value
  return map(value, 0, 1023, 0, 100);     // Scaling temperature
}