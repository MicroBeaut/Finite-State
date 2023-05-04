/*
  Sketch:   AuCP_Analog-High-Alarm.ino
  Created:  3-May-2023
  Author:   MicroBeaut (μB)
  GitHub:   https://github.com/MicroBeaut/Finite-State
*/

#include "FiniteState.h"

#define processValuePin A0  // Define the Process value input pin.
#define normalPin       6   // Define the normal status output pin.
#define preAlarmPin     5   // Define the pre-alarm status output pin.
#define highAlarmPin    4   // Define the high-alarm status output pin.

enum Operator {
  LT,           // Less than operator
  LE,           // Less than or equal operator
  GT,           // Greater than operator
  GE            // Greater than or equal operator
};

const long setpoint = 85; // Alarm Setpoint
const long deadband = 5;  // Alarm Deadband

Operator operators [] = {GE, GE, LT};                           // Comparison operators
long setpoints [] = {setpoint, setpoint, setpoint - deadband};  // Comparison setpoints

const long AnalogRead();                                              // Declare Analog Read Function
void ProcessAlarmStatus(bool normal, bool preAlarm, bool highAlarm);  // Declare Process Alarm Status Function

long processValue;                                                    // Declare processValue variable

/*
  ____________________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                           |
  |___________________________________________________________________________________________________________________________________________________|
  |             |       |                   | Next-State  | Next-State  |                 |                       |   Delay-Time    |                 |
  | State       |  Id   | Predicate         |   Fase      |   True      | Process         | Event                 | (milliseconds)  | Timer-Type      |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  | NORMAL      |  0	  | AnalogPredicate   |      0      |      1      | nullptr         | EventOnActionChanged  |               - | -               |
  | PRE_ALARM   |  1	  | AnalogPredicate   |      0      |      2      | nullptr         | EventOnActionChanged  |               - | -               |
  | HIGH_ALARM  |  2	  | AnalogPredicate   |      2      |      0      | nullptr         | EventOnActionChanged  |               - | -               |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

bool AnalogPredicate(id_t id);  // Declare analog predicate Function
void NormalProcess(id_t id);    // Declare normal process Function
void PreAlarmProcess(id_t id);  // Declare pre-alarm process Function
void HighAlarmProcess(id_t id); // Declare high-alarm process Function

enum AnalogState : id_t {
  NORMAL,
  PRE_ALARM,
  HIGH_ALARM
};

#define alarmDelay 3000         // Define alarm dalay

Transition transitions[] = {
  {AnalogPredicate, NORMAL, PRE_ALARM, NormalProcess},                                      // State-0 - NextF = 0, NextT = 1
  {AnalogPredicate, NORMAL, HIGH_ALARM, PreAlarmProcess, nullptr, alarmDelay, TRUE_TIMER},  // State-1 - NextF = 0, NextT = 2
  {AnalogPredicate, HIGH_ALARM, NORMAL, HighAlarmProcess}                                   // State-2 - NextF = 2, NextT = 0
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition);               // Number of Transitions

FiniteState finiteStateMachine(transitions, numberOfTransitions);                           // Finite-State Object

void setup() {
  pinMode(normalPin, OUTPUT);             // Set the normal LED pin mode
  pinMode(preAlarmPin, OUTPUT);           // Set the pre-alarm LED pin mode
  pinMode(highAlarmPin, OUTPUT);          // Set the hith-alarm LED= pin mode
  finiteStateMachine.begin(NORMAL);       // FSM begins with Initial Transition Id 0
}

void loop() {
  finiteStateMachine.execute();           // Execute the FSM
  processValue = AnalogRead();            // Read processValue
}

void NormalProcess(id_t id) {
  ProcessAlarmStatus(true, false, false); // Update process alarm status
}

void PreAlarmProcess(id_t id) {
  ProcessAlarmStatus(false, true, false); // Update process alarm status
}

void HighAlarmProcess(id_t id) {
  ProcessAlarmStatus(false, true, true);  // Update process alarm status
}

bool AnalogPredicate(id_t id) {
  bool value;
  switch (operators[id]) {
    case GE:
      value = processValue >= setpoints[id];  // Compare process value with setpoint
      break;
    case LT:
      value = processValue < setpoints[id];   // Compare process value with setpoint
      break;
    default:
      value = false;
      break;
  }
  return value;
}

void ProcessAlarmStatus(bool normal, bool preAlarm, bool highAlarm) {
  digitalWrite(normalPin, normal);        // Update normal status
  digitalWrite(preAlarmPin, preAlarm);    // Update pre-alarm status
  digitalWrite(highAlarmPin, highAlarm);  // Update high-alarm status
}

const long AnalogRead() {
  long value = analogRead(processValuePin); // Read Process Value
  return map(value, 0, 1023, 0, 100);       // Scaling processValue
}