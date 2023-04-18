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
  id_t current;                 // Current State
  id_t next;                    // Next State
  StateFunc stateFunc;          // State Function
  EventFunc eventFunc;          // Event Function
  unsigned long delayTime;      // Delay Time
  bool manualStart;             // Manual Start
} Transition;


class FiniteState {
  private:
    Transition *_transitions;       // Tranistion Pointer
    uint8_t _numberOfTransitions;   // Number of Transitions
    State _state;                   // State
    EventArgs _eventArgs;           // Event Argument
    bool _initial;                  // Initial State

    bool _startTimer;               // Start Timer
    unsigned long _startTime;       // Start Time
    unsigned long _elapsedTime;     // Elapsed Time

    void InternalTransition();
    void InternalSetAction(const Events event);
    void InternalSetId(const id_t id);
    bool InternalIDBAD(const id_t id);
    void InternalAutoStartTimer(const id_t id);
    bool InternalTimer(const id_t id);
    const uint8_t InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max);
  public:
    uint8_t &id;
    uint8_t &size;

    FiniteState(Transition *transitions, const uint8_t numberOfTransitions);
    void begin(const id_t id);
    void transition(const id_t id);
    void execute();
    void delayTime(const id_t id, unsigned long msDelayTime);
    void manualStart(const id_t id, const bool manualMode);
    bool isManualStart(const id_t id);
    void startTimer();
    void stopTimer();
    bool isStart();
};

#endif // FINITESTATE_H