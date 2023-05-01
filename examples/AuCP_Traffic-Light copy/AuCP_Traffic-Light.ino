#include "FiniteState.h"

#define redLightPin     5
#define yellowLightPin  4
#define greenLightPin   3

uint8_t lightPins[] = {redLightPin, greenLightPin, yellowLightPin}; // Define an array of light pins.
const uint8_t numberOfLights = sizeof(lightPins) / sizeof(uint8_t); // Calculate the number of lights.

typedef struct {
  unsigned long delayTime;
  unsigned long startTime;
} Timer;

Timer delayTimes[] = {
  {5000},   // RED Delay Time 5 seconds
  {10000},  // GREEN Delay Time 10 seconds
  {3000},   // YELLOW Delay Time 3 seconds
};

bool DelayTimePredicate(id_t id); // Predicate (Input)
void EventOnActionChanged(EventArgs e);    // Event State

Transition transitions[] = {
  {DelayTimePredicate, 0, 1, nullptr, EventOnActionChanged}, // State-1 - NextF = 0, NextT = 1
  {DelayTimePredicate, 1, 2, nullptr, EventOnActionChanged}, // State-2 - NextF = 1, NextT = 2
  {DelayTimePredicate, 2, 0, nullptr, EventOnActionChanged}, // State-3 - NextF = 2, NextT = 0
};
const uint8_t numberOftransitions = sizeof(transitions) / sizeof(Transition); // Calculate the number of transitions.

FiniteState finiteStateMachine(transitions, numberOftransitions);             // Define Finite-State Object

void setup() {
  for (uint8_t index = 0; index < numberOfLights; index ++) {
    pinMode(lightPins[index], OUTPUT);    // Set Pin Mode
    digitalWrite(lightPins[index], LOW);  // Set Light with the LOW state.
  }
  finiteStateMachine.begin(0);            // FSM begins with Initial Transition Id 0
}

void loop() {
  finiteStateMachine.execute();  // Execute the FSM
}

bool DelayTimePredicate(id_t id) {
  return (millis() - delayTimes[id].startTime >= delayTimes[id].delayTime); // Determine Time Delay
}

void EventOnActionChanged(EventArgs e) {
  switch (e.action) {
    case ENTRY:
      delayTimes[e.id].startTime  = millis(); // Reload start time
      digitalWrite(lightPins[e.id], HIGH);    // Set Light with the HIGH state.
      break;
    case EXIT:
      digitalWrite(lightPins[e.id], LOW);     // Set Light with the LOW state.
      break;
  }
}