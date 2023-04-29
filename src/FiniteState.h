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
  PROCESS,
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

typedef bool (*Predicate)(id_t);          //  Predicate Function Pointer
typedef void (*Process)(State);           //  Process Function Pointer
typedef void (*EventHandler)(EventArgs);  //  Event Handler Function Pointer

typedef struct {
  Predicate predicate;        // Predicate Function
  id_t nextF;                 // Next State on FALSE
  id_t nextT;                 // Next State on TRUE
  Process process;            // Process Function
  EventHandler eventHandler;  // Event Function
} Transition;

class FiniteState {
  private:
    Transition *_transitions;       // Tranistion Pointer
    uint8_t _numberOfTransitions;   // Number of Transitions
    State _state;                   // State
    EventArgs _eventArgs;           // Event Argument
    bool _initial;                  // Initial State

    void InternalPredicate();
    void InternalEventHandler(const Events e);
    void InternalProcess();
    void InternalExitEntry(const id_t id);
    void InternalEntry(const id_t id);
    void InternalExit();
    const uint8_t InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max);
  public:
    id_t &id;
    uint8_t &size;

    FiniteState(Transition *transitions, const uint8_t numberOfTransitions);
    void begin(const id_t id);
    void transition(const id_t id);
    void execute();
};

#endif // FINITESTATE_H