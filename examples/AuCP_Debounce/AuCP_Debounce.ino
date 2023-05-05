/*
  AuCP_Debounce.ino
  Created:  5-May-2023
  Author:   MicroBeaut
  GitHub:   https://github.com/MicroBeaut/Finite-State
*/

#include "FiniteState.h"

#define buttonPin A0  // Define the Button input pin.
#define ledPin    7   // Define the LED output pin.

/*
  __________________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                         |
  |_________________________________________________________________________________________________________________________________________________|
  |             |       |                 | Next-State  | Next-State  |                 |                       |   Delay-Time    |                 |
  | State       |  Id   | Predicate       |   Fase      |   True      | Process         | Event                 | (milliseconds)  | Timer-Type      |
  |_____________|_______|_________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  | RELEASED    |  0	  | ButtonPredicate |      0      |      1      | ReleasedProcess | -                   	|               - | -               |
  | DEBOUNCE_T  |  1	  | ButtonPredicate |      0      |      2      | -               | -                   	|              10 | TRUE_TIMER      |
  | PRESSED     |  2	  | ButtonPredicate |      3      |      2      | PressedProcess  | -                   	|               - | -               |
  | DEBOUNCE_F  |  3	  | ButtonPredicate |      0      |      2      | -               | -                   	|              10 | FALSE_TIMER     |
  |_____________|_______|_________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

bool ButtonPredicate(id_t id);  // Declare Read Button Predicate function
void ReleasedProcess(id_t id);  // Declare Released Process function
void PressedProcess(id_t id);   // Declare Pressed Process function

enum DebounceState : id_t {
  RELEASED,
  DEBOUNCE_T,
  PRESSED,
  DEBOUNCE_F
};

#define debounce 10                                                             // Debounce Delay 10 milliseconds

Transition transitions[] = {
  {ButtonPredicate, RELEASED, DEBOUNCE_T, ReleasedProcess},                     // State-0 - NextF = 0, NextT = 1
  {ButtonPredicate, RELEASED, PRESSED, nullptr, nullptr, debounce, TRUE_TIMER}, // State-1 - NextF = 0, NextT = 2
  {ButtonPredicate, DEBOUNCE_F, PRESSED, PressedProcess},                       // State-2 - NextF = 3, NextT = 2
  {ButtonPredicate, RELEASED, PRESSED, nullptr, nullptr, debounce, FALSE_TIMER} // State-3 - NextF = 0, NextT = 2
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition);   // Calculate the number of transitions.

FiniteState debounceFS(transitions, numberOfTransitions);                       // Finite-State Object
bool buttonState;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);   // Set the Button input mode
  pinMode(ledPin, OUTPUT);            // Set the LED output pin mode
  debounceFS.begin(RELEASED);         // FSM begins with Initial Transition Id 0
}

void loop() {
  debounceFS.execute();               // Execute the FSM
  digitalWrite(ledPin, buttonState);  // Set LED with the button State.
}

bool ButtonPredicate(id_t id) {
  return !digitalRead(buttonPin);     // Read Button value.
}

void ReleasedProcess(id_t id) {
  buttonState = false;                // Set the Button state with false value.
}

void PressedProcess(id_t id) {
  buttonState = true;                 // Set the Button state with true value.
}