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

#define id_t uint8_t
#define BADID(id,maxId) (id<maxId?false:true)

enum Events : int8_t {
  LOOP,
  EXIT,
  ENTRY,
};

typedef struct {
  id_t id;          // State id
  Events event;     // Event State
} EventArgs;

typedef struct {
  id_t id;          // State id
  bool firstScan;   // First Scan when State Activated
} State;

typedef bool (*PredicateFunc)(id_t);    //  Predicate Function Pointer
typedef void (*StateFunc)(State);       //  State Function Pointer
typedef void (*EventFunc)(EventArgs);   //  Event Function Pointer

typedef struct {
  PredicateFunc predicateFunc;  // Predicate Function
  id_t nextF;                   // Nest State on FALSE
  id_t nextT;                   // Next State on TRUE
  StateFunc stateFunc;          // State Function
  EventFunc eventFunc;          // Event Function
} Transition;


class FiniteState {
  private:
    Transition *_transitions;       // Tranistion Pointer
    uint8_t _numberOfTransitions;   // Number of Transitions
    State _state;                   // State
    EventArgs _eventArgs;           // Event Argument
    bool _initial;                  // Initial State

    void InternalTransition();
    void InternalSetAction(const Events event);
    void InternalSetId(const id_t id);
    const uint8_t InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max);
  public:
    uint8_t &id;
    uint8_t &size;

    FiniteState(Transition *transitions, const uint8_t numberOfTransitions);
    void begin(const id_t id);
    void transition(const id_t id);
    void execute();
};

#endif // FINITESTATE_H