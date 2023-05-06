/*
  FiniteState.cpp
  Created:  14-Apr-2023
  Author:   MicroBeaut

  MIT License
  Copyright (c) 2023 MicroBeaut
*/

#include "FiniteState.h"

FiniteState::FiniteState(Transition *transitions, const uint8_t numberOfTransitions): size(_size), id(_eventArgs.id) {
  _size = this->InternalLimit(numberOfTransitions, STATE_TRANSITION_MIN, STATE_TRANSITION_MAX);
  _transitions = transitions;
  _eventArgs.id = 0;
  _eventArgs.action = NONE;
}

void FiniteState::begin(const id_t id) {
  if (this->InternalBadId(id)) return;
  this->InternalEntryAction(id);
}

void FiniteState::transition(const id_t id) {
  if (this->InternalBadId(id)) return;
  this->InternalNextStateAction(id);
}

void FiniteState::execute() {
  this->InternalTimerCondition();
  this->InternalProcess();
}

void FiniteState::InternalTimerCondition() {
  TimerType timerType = _transitions[_eventArgs.id].timerType;
  switch (timerType) {
    case NOT_USED:
      this->InternalNotUsedTimerCondition();
      break;
    case TRANS_TIMER:
      this->InternalTransitionTimerCondition();
      break;
    case PREDIC_TIMER:
      this->InternalPredicateTimerCondition();
      break;
    case FALSE_TIMER:
      this->InternalFalseStateTimerCondition();
      break;
    case TRUE_TIMER:
      this->InternalTrueStateTimerCondition();
      break;
  }
}

void FiniteState::InternalProcess() {
  if (_transitions[_eventArgs.id].process) {
    _transitions[_eventArgs.id].process(_eventArgs.id);
  }
}

void FiniteState::InternalNotUsedTimerCondition() {
  TriState predicateState = this->InternalPredicate();
  this->InternalNextState(predicateState);
}

void FiniteState::InternalTransitionTimerCondition() {
  TriState timerState = this->InternalTimer();
  switch (timerState) {
    case True:
      this->InternalNextState(True);
      break;
  }
}

void FiniteState::InternalPredicateTimerCondition() {
  TriState timerState = this->InternalTimer();
  switch (timerState) {
    case True:
      TriState predicateState = this->InternalPredicate();
      this->InternalNextState(predicateState);
      break;
  }
}

void FiniteState::InternalFalseStateTimerCondition() {
  TriState predicateState = this->InternalPredicate();
  TriState timerState = this->InternalTimer();
  switch (predicateState) {
    case False:
      if (timerState == True) this->InternalNextState(False);
      break;
    case True:
      this->InternalNextState(True);
      break;
  }
}

void FiniteState::InternalTrueStateTimerCondition() {
  TriState predicateState = this->InternalPredicate();
  TriState timerState = this->InternalTimer();
  switch (predicateState) {
    case False:
      this->InternalNextState(False);
      break;
    case True:
      if (timerState == True) this->InternalNextState(True);
      break;
  }
}

const TriState FiniteState::InternalPredicate() {
  if (_transitions[_eventArgs.id].predicate) {
    bool state = _transitions[_eventArgs.id].predicate(_eventArgs.id);
    return state ? True : False;
  }
  return Unspecified;
}

void FiniteState::InternalNextState(TriState state) {
  switch (state) {
    case False:
      this->InternalNextStateAction(_transitions[_eventArgs.id].nextF);
      break;
    case True:
      this->InternalNextStateAction(_transitions[_eventArgs.id].nextT);
      break;
  }
}

void FiniteState::InternalNextStateAction(const id_t id) {
  if (_eventArgs.id == id) return;
  this->InternalExitAction();
  this->InternalEntryAction(id);
}

void FiniteState::InternalEntryAction(const id_t id) {
  _startTime = micros();
  _timeout = false;
  _eventArgs.id = id;
  this->InternalEventHandler(ENTRY);
}

void FiniteState::InternalExitAction() {
  this->InternalEventHandler(EXIT);
}

void FiniteState::InternalEventHandler(const Action e) {
  if (_eventArgs.action == e) return;
  _eventArgs.action = e;
  if (_transitions[_eventArgs.id].eventHandler) {
    _transitions[_eventArgs.id].eventHandler(_eventArgs);
  }
  _eventArgs.action = NONE;
}

const TriState FiniteState::InternalTimer() {
  if (_transitions[_eventArgs.id].delayTime < 1) return Unspecified;
  bool timeout = this->InternalTimeout();
  return timeout ? True : False;
}

const bool FiniteState::InternalTimeout() {
  if (_timeout) return true;
  time_t elapsedTime = micros() - _startTime;
  if (elapsedTime < MS2US(_transitions[_eventArgs.id].delayTime)) return false;
  _timeout = true;
  return true;
}

const bool FiniteState::InternalBadId(id_t id) {
  if (id < _size) return false;
  return true;
}

const uint8_t FiniteState::InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}