/*
  Sketch:   AuCP_Fan-Control-With-A-Thermostat.ino
  Created:  17-Apr-2023
  Author:   MicroBeaut (μB)
  GitHub:   https://github.com/MicroBeaut/Finite-State
*/

#include "FiniteState.h"

#define thermostatPin   A0
#define startStatusPin  5
#define stopStatusPin   6

/*
  ____________________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                           |
  |___________________________________________________________________________________________________________________________________________________|
  |             |       |                   | Next-State  | Next-State  |                 |                       |   Delay-Time    |                 |
  | State       |  Id   | Predicate         |   Fase      |   True      | Process         | Event                 | (milliseconds)  | Timer-Type      |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  | STOP        |  0	  | HighTempPredicate |      0      |      1      | FanStopProcess  | -                     |               - | -               |
  | START       |  1	  | LowTempPredicate  |      1      |      0      | FanStartProcess | -	                    |               - | -               |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

void FanStartProcess(id_t id);
void FanStopProcess(id_t id);
bool HighTempPredicate(id_t state);
bool LowTempPredicate(id_t state);

enum FanState : id_t {
  STOP,
  START
};

Transition transitions[] = {
  {HighTempPredicate, STOP, START, FanStopProcess},                           // State-0 - NextF = 0, NextT = 1
  {LowTempPredicate, START, STOP, FanStartProcess}                            // State-1 - NextF = 1, NextT = 0
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition); // Number of Transitions

FiniteState finiteStateMachine(transitions, numberOfTransitions);             // Finite-State Object

const long ThermostatRead();

void setup() {
  pinMode(startStatusPin, OUTPUT);        // Set the start status pin mode
  pinMode(stopStatusPin, OUTPUT);         // Set the sotp status pin mode
  finiteStateMachine.begin(STOP);         // FSM begins with Initial Transition Id 0
}

void loop() {
  finiteStateMachine.execute();           // Execute the FSM
}

bool HighTempPredicate(id_t state) {
  return ThermostatRead() >= 40;          // Determine Fan Start Action
}

bool LowTempPredicate(id_t state) {
  return ThermostatRead() <= 30;          // Determine Fan Stop Action
}

void FanStartProcess(id_t id) {
  digitalWrite(stopStatusPin, false);     // Update fan stop status
  digitalWrite(startStatusPin, true);     // Update fan start status
}

void FanStopProcess(id_t id) {
  digitalWrite(startStatusPin, false);    // Update fan start status
  digitalWrite(stopStatusPin, true);      // Update fan stop status
}

const long ThermostatRead() {
  long value = analogRead(thermostatPin); // Read Pushbutton Value
  return map(value, 0, 1023, 0, 100);     // Scaling temperature
}