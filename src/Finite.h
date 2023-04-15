/*
  Finite.h
  Created:  14-Apr-2023
  Author:   MicroBeaut

  MIT License
  Copyright (c) 2023 MicroBeaut
*/

#ifndef FINITE_H
#define FINITE_H

#include "Arduino.h"

#define STATE_TRANSITION_MIN 1
#define STATE_TRANSITION_MAX 36

enum Actions : int8_t {
  NONE = - 1,
  EXIT = 0,
  ENTRY = 1
};

typedef struct {
  uint8_t id;             // State id
  Actions action;         // State Action
  bool firstScan;         // First Scan when Activated
} State;

typedef bool (*InputFunc)(State);
typedef void (*TransitionFunc)(State);

typedef struct {
  TransitionFunc transition;  // Transition Function
  InputFunc input;            // Input Function
  uint8_t current;            // Current State
  uint8_t next;               // Next State
} Transition;


class Finite {
  private:
    Transition *_transitions;     // Tranistion
    uint8_t _numberOfTransitions; // Number of Transition
    State _state;                 // State
    uint8_t _prevId;              // Previous id
    uint8_t _prevAction;          // Previous action

    bool  _initial;               // Initial State
    bool _end;

    bool InternalEnd();
    void InternalTransition();
    void InternalAction();
    const uint8_t InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max);
  public:
    uint8_t &size;

    Finite(Transition *transitions, const uint8_t numberOfTransition);
    void begin();
    void state(const uint8_t state);
    void execute();
    void end();
};

#endif // FINITE_H