/*
  AuCP_Blink.ino
  Created:  5-May-2023
  Author:   MicroBeaut
  GitHub:   https://github.com/MicroBeaut/Finite-State
*/

#include "FiniteState.h"

/*
  ____________________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                           |
  |___________________________________________________________________________________________________________________________________________________|
  |             |       |                   | Next-State  | Next-State  |                 |                       |   Delay-Time    |                 |
  | State       |  Id   | Predicate         |   Fase      |   True      | Process         | Event                 | (milliseconds)  | Timer-Type      |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  | LED_OFF     |  0	  | -                 |      0      |      1      | TurnOffProcess  | -                     |             500 | TRANS_TIMER     |
  | LED_ON      |  1	  | -                 |      1      |      0      | TrunOnProcess   | -                     |           1,000 | TRANS_TIMER     |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

void TrunOnProcess(id_t id);     // Declare Turn LED On Process function
void TurnOffProcess(id_t id);    // Declare Turn LED Off Process function

enum LedState : id_t {
  LED_OFF,
  LED_ON
};

Transition transitions[] = {
  {nullptr, LED_OFF, LED_ON, TurnOffProcess, nullptr, 500, TRANS_TIMER},      // State-0 - NextF = 0, NextT = 1
  {nullptr, LED_ON, LED_OFF, TrunOnProcess, nullptr, 1000, TRANS_TIMER}       // State-1 - NextF = 1, NextT = 0
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition); // Calculate the number of transitions.

FiniteState blinkFS(transitions, numberOfTransitions);                        // Finite-State Object

void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);     // Set the LED_BUILTIN pin mode
  blinkFS.begin(LED_OFF);           // FSM begins with Initial Transition Id 0
}

void loop() {
  blinkFS.execute();                // Execute the FSM
}

void TrunOnProcess(id_t id) {
  digitalWrite(LED_BUILTIN, HIGH);  // Turn on the LED.
}

void TurnOffProcess(id_t id) {
  digitalWrite(LED_BUILTIN, LOW);   // Turn off the LED.
}