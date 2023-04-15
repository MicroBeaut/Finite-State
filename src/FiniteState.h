/*
  FiniteState.h
  Created:  14-Apr-2023
  Author:   MicroBeaut

  MIT License
  Copyright (c) 2023 MicroBeaut
*/

#ifndef FINITESTATE_H
#define FINITESTATE_H

#include "Arduino.h"

#define STATE_TRANSITION_MIN 1
#define STATE_TRANSITION_MAX 36

enum Actions : int8_t {
  NONE = - 1,
  EXIT = 0,
  ENTRY = 1,
};

typedef struct {
  uint8_t id;             // State id
  Actions action;         // State Action
  bool firstScan;         // First Scan when Activated
} State;

typedef void (*TransitionFunc)(State);  //  Tranistion Function Pointer 
typedef bool (*InputFunc)(State);       //  Input Function Pointer
typedef void (*EventFunc)(State);       //  Event Function Pointer

typedef struct {
  TransitionFunc transitionFunc;  // Transition Function
  InputFunc inputFun;             // Input Function
  uint8_t current;                // Current State
  uint8_t next;                   // Next State
  EventFunc eventFunc;            // Event Function
} Transition;


class FiniteState {
  private:
    Transition *_transitions;     // Tranistion
    uint8_t _numberOfTransitions; // Number of Transition
    State _state;                 // State
    bool _initial;                // Initial State

    void InternalTransition();
    void InternalSetAction(Actions action);
    void InternalSetId(uint8_t id);
    const uint8_t InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max);
  public:
    uint8_t &size;

    FiniteState(Transition *transitions, const uint8_t numberOfTransition);
    void begin(const uint8_t id);
    void transition(const uint8_t id);
    void execute();
};

#endif // FINITESTATE_H