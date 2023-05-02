#include "FiniteState.h"

#define BUTTON  A0  // Define the button input pin.
#define LED     7   // Define the LED output pin.

#define debounce 10 // Debounce Delay 10 milliseconds

/*
  __________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                 |
  |_________________________________________________________________________________________________________________________________________|
  |             |       |                 | Next-State  | Next-State  |                 | Event         |   Delay-Time    | Timer-Type      |
  | Name        |  Id   | Predicate       |   Fase      |   True      | Process         |               | (milliseconds)  |                 |
  |_____________|_______|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
  | Released    |  0	  |  ReadButton     |      0      |      1      | Released        | -	            |              -  |  TRUE_TIMER     |
  | DebounceT   |  1	  |  ReadButton     |      0      |      2      | -	              | -	            |             10  |  -              |
  | Pressed     |  2	  |  ReadButton     |      3      |      2      | Pressed         | -	            |              -  |  FALSE_TIMER    |
  | DebounceF   |  3	  |  ReadButton     |      0      |      2      | -	              | -	            |             10  |  -              |
  |_____________|_______|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
*/

bool ReadButton(id_t id);   // Declare Read Button Predicate function
void Released(id_t id);     // Declare Released Process function
void Pressed(id_t id);      // Declare Pressed Process function

Transition transitions[] = {
  {ReadButton, 0, 1, Released},                               // State-0 - NextF = 0, NextT = 1
  {ReadButton, 0, 2, nullptr, nullptr, debounce, TRUE_TIMER}, // State-1 - NextF = 0, NextT = 2
  {ReadButton, 3, 2, Pressed},                                // State-2 - NextF = 3, NextT = 2
  {ReadButton, 0, 2, nullptr, nullptr, debounce, FALSE_TIMER} // State-3 - NextF = 0, NextT = 2
};
const uint8_t NumberOfTransitions = 4;                        // Number Of Transitions

FiniteState debounceFS(transitions, NumberOfTransitions);     // Finite-State Object
bool state;
void setup() {
  pinMode(BUTTON, INPUT_PULLUP);    // Set the Button pin mode
  pinMode(LED, OUTPUT);             // Set the LED pin mode
  debounceFS.begin(0);              // FSM begins with Initial Transition Id 0
}

void loop() {
  debounceFS.execute();             // Execute the FSM
  digitalWrite(LED, state);         // Set LED with the state.
}

bool ReadButton(id_t id) {
  return !digitalRead(BUTTON);      // Read Button value.
}

void Released(id_t id) {
  state = false;                    // Set the state with false value.
}

void Pressed(id_t id) {
  state = true;                     // Set the state with true value.
}