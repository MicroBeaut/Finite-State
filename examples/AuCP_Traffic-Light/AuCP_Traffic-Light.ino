/*
  Sketch:   AuCP_Traffic-Light.ino
  Created:  17-Apr-2023
  Author:   MicroBeaut (μB)
  GitHub:   https://github.com/MicroBeaut/Finite-State#traffic-light-with-customized-timer-not_used
*/

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

/*
  ____________________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                           |
  |___________________________________________________________________________________________________________________________________________________|
  |             |       |                   | Next-State  | Next-State  |                 |                       |   Delay-Time    |                 |
  | State       |  Id   | Predicate         |   Fase      |   True      | Process         | Event                 | (milliseconds)  | Timer-Type      |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  | RED         |  0	  | TimePredicate     |      0      |      1      | -               | EventOnActionChanged  |               - | -               |
  | GREEN       |  1	  | TimePredicate     |      1      |      2      | -               | EventOnActionChanged  |               - | -               |
  | YELLOW      |  2	  | TimePredicate     |      2      |      0      | -               | EventOnActionChanged  |               - | -               |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

bool TimePredicate(id_t id);              // Predicate (Input)
void EventOnActionChanged(EventArgs e);   // Event State

enum TraficState {
  RED,
  GREEN,
  YELLOW
};

Transition transitions[] = {
  {TimePredicate, RED, GREEN, nullptr, EventOnActionChanged},                 // State-1 - NextF = 0, NextT = 1
  {TimePredicate, GREEN, YELLOW, nullptr, EventOnActionChanged},              // State-2 - NextF = 1, NextT = 2
  {TimePredicate, YELLOW, RED, nullptr, EventOnActionChanged},                // State-3 - NextF = 2, NextT = 0
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition); // Calculate the number of transitions.

FiniteState finiteStateMachine(transitions, numberOfTransitions);             // Define Finite-State Object

void setup() {
  for (uint8_t index = 0; index < numberOfLights; index ++) {
    pinMode(lightPins[index], OUTPUT);    // Set Pin Mode
    digitalWrite(lightPins[index], LOW);  // Set Light with the LOW state.
  }
  finiteStateMachine.begin(RED);          // FSM begins with Initial Transition Id 0
}

void loop() {
  finiteStateMachine.execute();           // Execute the FSM
}

bool TimePredicate(id_t id) {
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