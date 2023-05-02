#include "FiniteState.h"
#include "RepeatButton.h"

#define COIN      A0  // Define the Coin input pin.
#define PUSH      A1  // Define the Push input pin.

#define LOCKED    7   // Define the Locked state output pin.
#define UNLOCKED  6   // Define the Unlocked state output pin. 

/*
  __________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                 |
  |_________________________________________________________________________________________________________________________|
  |  Id | Predicate       | Next-State  | Next-State  | Process         | Event         |   Delay-Time    | Timer-Type      |
  |     |                 |   Fase      |   True      |                 |               | (milliseconds)  |                 |
  |_____|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
  |  0	|  CoinPredicate	|      0      |      1      | LockedProcess	  | -	            |               - |  -              |
  |  1	|  PushPredicate	|      1      |      0      |	UnlockedProcess | -	            |               - |  -              |
  |_____|_________________|_____________|_____________|_________________|_______________|_________________|_________________|
*/

bool CoinPredicate(id_t id);              // Declare Coin Predicate function
bool PushPredicate(id_t id);              // Declare Push Predicate function

void LockedProcess(id_t id);              // Declare Locked Process function
void UnlockedProcess(id_t id);            // Declare Unlocked Process function

Transition transitions[] = {
  {CoinPredicate, 0, 1, LockedProcess},   // State-0 - NextF = 0, NextT = 1
  {PushPredicate, 1, 0, UnlockedProcess}  // State-1 - NextF = 1, NextT = 0
};
const uint8_t NumberOfTransitions = 2;    // Number Of Transitions

FiniteState coinOperatedTurnstile(transitions, NumberOfTransitions);  // Finite-State Object
RepeatButton coin;                                                    // Declare the Coin RepeatButton object
RepeatButton push;                                                    // Declare the Push RepeatButton object

void setup() {
  coin.buttonMode(COIN, INPUT_PULLUP);    // Set the Coin input pin mode
  push.buttonMode(PUSH, INPUT_PULLUP);    // Set the Push input pin mode
  pinMode(LOCKED, OUTPUT);                // Set the Locked state pin mode
  pinMode(UNLOCKED, OUTPUT);              // Set the Unlocked state pin mode

  coinOperatedTurnstile.begin(0);         // FSM begins with Initial Transition Id 0
}

void loop() {
  coin.repeatButton();                    // Executing the Coin repeat button function
  push.repeatButton();                    // Executing the Push repeat button function
  coinOperatedTurnstile.execute();        // Execute the FSM
}

bool CoinPredicate(id_t id) {
  return coin.isPressed();                // Predicate putting a coin.
}

bool PushPredicate(id_t id) {
  return push.isPressed();                // Predicate pushing the arm.
}

void LockedProcess(id_t id) {
  digitalWrite(LOCKED, HIGH);             // Turn on the locked position status.
  digitalWrite(UNLOCKED, LOW);            // Turn off the unlocked position status.
}

void UnlockedProcess(id_t id) {
  digitalWrite(LOCKED, LOW);              // Turn off the locked position status.
  digitalWrite(UNLOCKED, HIGH);           // Turn on the unlocked position status.
}