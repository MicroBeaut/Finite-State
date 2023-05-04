#include "FiniteState.h"

#define buttonPin A0  // Define the Button input pin.
#define ledPin    7   // Define the LED output pin.

/*
  __________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                 |
  |_________________________________________________________________________________________________________________________________________|
  |             |       |                 | Next-State  | Next-State  |                 | Event         |   Delay-Time    | Timer-Type      |
  | Name        |  Id   | Predicate       |   Fase      |   True      | Process         |               | (milliseconds)  |                 |
  |_____________|_______|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
  | Released    |  0	  | ButtonPredicate |      0      |      1      | ReleasedProcess | -	            |              -  |  TRUE_TIMER     |
  | DebounceT   |  1	  | ButtonPredicate |      0      |      2      | -	              | -	            |             10  |  -              |
  | Pressed     |  2	  | ButtonPredicate |      3      |      2      | PressedProcess  | -	            |              -  |  FALSE_TIMER    |
  | DebounceF   |  3	  | ButtonPredicate |      0      |      2      | -	              | -	            |             10  |  -              |
  |_____________|_______|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
*/

bool ButtonPredicate(id_t id);  // Declare Read Button Predicate function
void ReleasedProcess(id_t id);  // Declare Released Process function
void PressedProcess(id_t id);   // Declare Pressed Process function

#define debounce 10             // Debounce Delay 10 milliseconds

Transition transitions[] = {
  {ButtonPredicate, 0, 1, ReleasedProcess},                         // State-0 - NextF = 0, NextT = 1
  {ButtonPredicate, 0, 2, nullptr, nullptr, debounce, TRUE_TIMER},  // State-1 - NextF = 0, NextT = 2
  {ButtonPredicate, 3, 2, PressedProcess},                          // State-2 - NextF = 3, NextT = 2
  {ButtonPredicate, 0, 2, nullptr, nullptr, debounce, FALSE_TIMER}  // State-3 - NextF = 0, NextT = 2
};
const uint8_t NumberOfTransitions = 4;                              // Number Of Transitions

FiniteState debounceFS(transitions, NumberOfTransitions);           // Finite-State Object
bool buttonState;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);   // Set the Button input mode
  pinMode(ledPin, OUTPUT);            // Set the LED output pin mode
  debounceFS.begin(0);                // FSM begins with Initial Transition Id 0
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