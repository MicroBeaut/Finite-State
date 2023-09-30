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

#define FINITE_STATE_VERSION "1.6.0"
#define FINITE_STATE_AUTHOR "MicroBeaut"

#define STATE_TRANSITION_MIN 1
#define STATE_TRANSITION_MAX 63

#define id_t uint8_t
#define time_t unsigned long

enum Action {
  NONE,
  ENTRY,
  EXIT
};

enum TimerType {
  NOT_USED,       // Not Used Timer
  TRANS_TIMER,    // Transition Timer
  PREDIC_TIMER,   // Predicate Timer
  FALSE_TIMER,    // False State Timer
  TRUE_TIMER,     // True State Timer
};

enum TriState {
  Unspecified = -1,
  False = 0,
  True = 1
};

typedef struct {
  id_t id;          // State id
  Action action;    // Action State
} EventArgs;

typedef bool (*Predicate)(id_t);          //  Predicate Function Pointer
typedef void (*Process)(id_t);            //  Process Function Pointer
typedef void (*EventHandler)(EventArgs);  //  Event Handler Function Pointer

typedef struct {
  Predicate predicate;                    // Predicate Function Pointer
  id_t nextF;                             // Next State on FALSE
  id_t nextT;                             // Next State on TRUE
  Process process;                        // Process Function Pointer
  EventHandler eventHandler;              // Event Function Pointer
  time_t delayTime;                       // Delay Time
  TimerType timerType;                    // Timer Type
} Transition;

class FiniteState {
  private:
    Transition *_transitions;             // Tranistion Pointer
    uint8_t _size;                        // Number of Transitions
    EventArgs _eventArgs;                 // Event Argument
    time_t _startTime;                    // Start Time
    bool _timeout;                        // Timeout State

    void InternalTimerCondition();
    void InternalProcess();

    void InternalNotUsedTimerCondition();
    void InternalTransitionTimerCondition();
    void InternalPredicateTimerCondition();
    void InternalFalseStateTimerCondition();
    void InternalTrueStateTimerCondition();

    const TriState InternalPredicate();

    void InternalNextState(TriState state);
    void InternalNextStateAction(const id_t id);

    void InternalEntryAction(const id_t id);
    void InternalExitAction();

    void InternalEventHandler(const Action e);

    const TriState InternalTimer();
    const bool InternalTimeout();

    const bool InternalBadId(id_t id);
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