#include "FiniteState.h"
#include "RepeatButton.h"

#define COIN      A0  // Define the Coin input pin.
#define PUSH      A1  // Define the Push input pin.

#define LOCKED    7   // Define the Locked state output pin.
#define UNLOCKED  6   // Define the Unlocked state output pin. 

/*
  __________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                         |
  |_________________________________________________________________________________________________________________________________|
  |  Id | Predicate       | Next-State  | Next-State  | Process         | Event                 |   Delay-Time    | Timer-Type      |
  |     |                 |   Fase      |   True      |                 |                       | (milliseconds)  |                 |
  |_____|_________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  |  0	|  inputPredicate	|      0      |      1      | -	              | EventOnActionChanged	|               - |  -              |
  |  1	|  inputPredicate	|      1      |      0      | -	              | EventOnActionChanged	|               - |  -              |
  |_____|_________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

bool inputPredicate(id_t id);             // Declare Coin Predicate function
void EventOnActionChanged(EventArgs e);   // Event On Action Changed

Transition transitions[] = {
  {inputPredicate, 0, 1, nullptr, EventOnActionChanged},  // State-0 - NextF = 0, NextT = 1
  {inputPredicate, 1, 0, nullptr, EventOnActionChanged}   // State-1 - NextF = 1, NextT = 0
};
const uint8_t NumberOfTransitions = 2;                    // Number Of Transitions

uint8_t inputPins[NumberOfTransitions] = {COIN, PUSH};                // Declare the Coin RepeatButton object
uint8_t outputPins[NumberOfTransitions] = {LOCKED, UNLOCKED};         // Declare the Coin RepeatButton object

FiniteState coinOperatedTurnstile(transitions, NumberOfTransitions);  // Finite-State Object
RepeatButton turnstileInputs[NumberOfTransitions];                    // Declare the Turnstile Inputs RepeatButton object

void setup() {
  for (uint8_t index = 0; index < NumberOfTransitions; index++) {
    turnstileInputs[index].buttonMode(inputPins[index], INPUT_PULLUP);  // Set the Turnstile repeat button pin mode
    pinMode(outputPins[index], OUTPUT);                                 // Set the Output state pin mode
  }
  coinOperatedTurnstile.begin(0);                                       // FSM begins with Initial Transition Id 0
}

void loop() {
  for (uint8_t index = 0; index < NumberOfTransitions; index++) {
    turnstileInputs[index].repeatButton();    // Executing the Turnstile repeat button function
  }
  coinOperatedTurnstile.execute();            // Execute the FSM
}

bool inputPredicate(id_t id) {
  return turnstileInputs[id].isPressed();     // Predicate putting a coin.
}

void EventOnActionChanged(EventArgs e) {
  switch (e.action) {
    case ENTRY:
      digitalWrite(outputPins[e.id], HIGH);   // Turn on the turnstile position status.
      break;
    case EXIT:
      digitalWrite(outputPins[e.id], LOW);    // Turn off the previous turnstile position status.
      break;
  }
}