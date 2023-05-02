#include "FiniteState.h"

/*
  __________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                 |
  |_________________________________________________________________________________________________________________________|
  |  Id | Predicate       | Next-State  | Next-State  | Process         | Event         |   Delay-Time    | Timer-Type      |
  |     |                 |   Fase      |   True      |                 |               | (milliseconds)  |                 |
  |_____|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
  |  0	|  -	            |      0      |      1      | TurnON	        | -	            |             500 |  TRANS_TIMER    |
  |  1	|  -	            |      1      |      0      |	TurnOFF         | -	            |           1,000 |  TRANS_TIMER    |
  |_____|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
*/

void TurnON(id_t id);     // Declare Turn LED On Process function
void TurnOFF(id_t id);    // Declare Turn LED Off Process function

Transition transitions[] = {
  {nullptr, 0, 1, TurnON, nullptr, 500, TRANS_TIMER},   // State-0 - NextF = 0, NextT = 1
  {nullptr, 1, 0, TurnOFF, nullptr, 1000, TRANS_TIMER}  // State-1 - NextF = 1, NextT = 0
};
const uint8_t NumberOfTransitions = 2;                  // Number Of Transitions

FiniteState blinkFS(transitions, NumberOfTransitions);  // Finite-State Object

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Set the LED_BUILTIN pin mode
  blinkFS.begin(0);                 // FSM begins with Initial Transition Id 0
}

void loop() {
  blinkFS.execute();                // Execute the FSM
}

void TurnON(id_t id) {
  digitalWrite(LED_BUILTIN, HIGH);  // Turn on the LED.
}

void TurnOFF(id_t id) {
  digitalWrite(LED_BUILTIN, LOW);   // Turn off the LED.
}