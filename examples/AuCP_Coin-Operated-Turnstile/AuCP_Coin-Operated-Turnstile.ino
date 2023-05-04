/*
  AuCP_Coin-Operated-Turnstile.ino
  Created:  4-May-2023
  Author:   MicroBeaut
  GitHub:   https://github.com/MicroBeaut/Finite-State
*/

#include "FiniteState.h"
#include "RepeatButton.h"

#define coinInputPin      A0    // Define the Coin input pin.
#define armInputPin       A1    // Define the Arm input pin.

#define lockedStatusPin   7     // Define the Locked state output pin.
#define unlockedStatusPin 6     // Define the Unlocked state output pin. 

/*
  ____________________________________________________________________________________________________________________________________________________
  |  State-Transition Table                                                                                                                           |
  |___________________________________________________________________________________________________________________________________________________|
  |             |       |                   | Next-State  | Next-State  |                 |                       |   Delay-Time    |                 |
  | State       |  Id   | Predicate         |   Fase      |   True      | Process         | Event                 | (milliseconds)  | Timer-Type      |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
  | LOCKED      |  0	  | CoinPredicate     |      0      |      1      | LockedProcess   | -                     |               - | -               |
  | UNLOCKED    |  1	  | ArmPredicate      |      1      |      0      | UnlockedProcess | -                     |               - | -               |
  |_____________|_______|___________________|_____________|_____________|_________________|_______________________|_________________|_________________|
*/

bool CoinPredicate(id_t id);    // Declare Coin Predicate function
bool ArmPredicate(id_t id);     // Declare Arm Predicate function

void LockedProcess(id_t id);    // Declare Locked Process function
void UnlockedProcess(id_t id);  // Declare Unlocked Process function

enum TurnstileState : id_t {
  LOCKED,
  UNLOCKED
};

Transition transitions[] = {
  {CoinPredicate, LOCKED, UNLOCKED, LockedProcess},                           // State-0 - NextF = 0, NextT = 1
  {ArmPredicate, UNLOCKED, LOCKED, UnlockedProcess}                           // State-1 - NextF = 1, NextT = 0
};
const uint8_t numberOfTransitions = sizeof(transitions) / sizeof(Transition); // Calculate the number of transitions.

FiniteState coinOperatedTurnstile(transitions, numberOfTransitions);          // Finite-State Object

RepeatButton coin;                                                            // Declare the Coin RepeatButton object
RepeatButton arm;                                                             // Declare the Arm RepeatButton object

void setup() {
  coin.buttonMode(coinInputPin, INPUT_PULLUP);  // Set the Coin input pin mode
  arm.buttonMode(armInputPin, INPUT_PULLUP);    // Set the Arm input pin mode
  pinMode(lockedStatusPin, OUTPUT);             // Set the Locked state pin mode
  pinMode(unlockedStatusPin, OUTPUT);           // Set the Unlocked state pin mode

  coinOperatedTurnstile.begin(LOCKED);          // FSM begins with Initial Transition Id 0
}

void loop() {
  coin.repeatButton();                    // Executing the Coin repeat button function
  arm.repeatButton();                     // Executing the Arm repeat button function
  coinOperatedTurnstile.execute();        // Execute the FSM
}

bool CoinPredicate(id_t id) {
  return coin.isPressed();                // Predicate putting a coin.
}

bool ArmPredicate(id_t id) {
  return arm.isPressed();                // Predicate pushing the arm.
}

void LockedProcess(id_t id) {
  digitalWrite(lockedStatusPin, HIGH);    // Turn on the locked position status.
  digitalWrite(unlockedStatusPin, LOW);   // Turn off the unlocked position status.
}

void UnlockedProcess(id_t id) {
  digitalWrite(lockedStatusPin, LOW);     // Turn off the locked position status.
  digitalWrite(unlockedStatusPin, HIGH);  // Turn on the unlocked position status.
}