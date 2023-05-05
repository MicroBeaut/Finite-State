/*
  AuCP_Coin-Operated-Turnstile-With-Event.ino
  Created:  4-May-2023
  Author:   MicroBeaut
  GitHub:   https://github.com/MicroBeaut/Finite-State#coin-operated-turnstile-with-predicate-and-event
*/

#include "FiniteState.h"
#include "RepeatButton.h"

#define coinInputPin      A0  // Define the Coin input pin.
#define armInputPin       A1  // Define the Push input pin.

#define lockedStatusPin   7   // Define the Locked state output pin.
#define unlockedStatusPin 6   // Define the Unlocked state output pin.

/*
  ____________________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                           |
  |___________________________________________________________________________________________________________________________________________________|
  |             |       |                   | Next-State  | Next-State  |                 |                       |   Delay-Time    |                 |
  | State       |  Id   | Predicate         |   Fase      |   True      | Process         | Event                 | (milliseconds)  | Timer-Type      |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  | LOCKED      |  0	  | CoinPredicate     |      0      |      1      | -               | EventOnActionChanged  |               - | -               |
  | UNLOCKED    |  1	  | ArmPredicate      |      1      |      0      | -               | EventOnActionChanged  |               - | -               |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

bool inputPredicate(id_t id);             // Declare Coin Predicate function
void EventOnActionChanged(EventArgs e);   // Event On Action Changed

enum TurnstileState : id_t {
  LOCKED,
  UNLOCKED
};

Transition transitions[] = {
  {inputPredicate, LOCKED, UNLOCKED, nullptr, EventOnActionChanged},            // State-0 - NextF = 0, NextT = 1
  {inputPredicate, UNLOCKED, LOCKED, nullptr, EventOnActionChanged}             // State-1 - NextF = 1, NextT = 0
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition);   // Calculate the number of transitions.

FiniteState coinOperatedTurnstile(transitions, numberOfTransitions);            // Finite-State Object

uint8_t inputPins[numberOfTransitions] = {coinInputPin, armInputPin};           // Declare the input pin array
uint8_t outputPins[numberOfTransitions] = {lockedStatusPin, unlockedStatusPin}; // Declare the output pin array
RepeatButton turnstileInputs[numberOfTransitions];                              // Declare the Turnstile Inputs RepeatButton object

void setup() {
  for (uint8_t index = 0; index < numberOfTransitions; index++) {
    turnstileInputs[index].buttonMode(inputPins[index], INPUT_PULLUP);  // Set the Turnstile repeat button pin mode
    pinMode(outputPins[index], OUTPUT);                                 // Set the Output state pin mode
  }
  coinOperatedTurnstile.begin(LOCKED);                                  // FSM begins with Initial Transition Id 0
}

void loop() {
  for (uint8_t index = 0; index < numberOfTransitions; index++) {
    turnstileInputs[index].repeatButton();    // Executing the Turnstile repeat button function.
  }
  coinOperatedTurnstile.execute();            // Execute the FSM.
}

bool inputPredicate(id_t id) {
  return turnstileInputs[id].isPressed();     // Predicate putting a coin and pushing the arm.
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