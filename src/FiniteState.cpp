/*
  FiniteState.cpp
  Created:  14-Apr-2023
  Author:   MicroBeaut

  MIT License
  Copyright (c) 2023 MicroBeaut
*/

#include "FiniteState.h"

FiniteState::FiniteState(Transition *transitions, const uint8_t numberOfTransitions): size(_numberOfTransitions), id(_state.id) {
  _numberOfTransitions = this->InternalLimit(numberOfTransitions, STATE_TRANSITION_MIN, STATE_TRANSITION_MAX);
  _transitions = transitions;
  _initial = false;
  _eventArgs.action = DURING;
}

void FiniteState::begin(const id_t id) {
  if (BADID(id, _numberOfTransitions)) return;
  _initial = true;
  this->InternalEntryAction(id);
  this->InternalDuringAction();
}

void FiniteState::transition(const id_t id) {
  if (BADID(id, _numberOfTransitions)) return;
  this->InternalNextStateAction(id);
}

void FiniteState::execute() {
  if (!_initial) return;
  Action action = _eventArgs.action;
  this->InternalTimerCondition();
  this->InternalProcess();
  if (_state.firstScan) _state.firstScan = false;
}

void FiniteState::InternalTimerCondition() {
  TimerType timerType = _transitions[_state.id].timerType;
  switch (timerType) {
    case NOT_USED:
      this->InternalNotUsedTimerCondition();
      break;
    case TRANS_TIMER:
      this->InternalTransitionTimerCondition();
      break;
    case PREDIC_TIMER:
      this->InternalInputTimerCondition();
      break;
    case FALSE_TIMER:
      this->InternalFalseStateTimerCondition();
      break;
    case TRUE_TIMER:
      this->InternalInputTimerCondition();
      break;
  }
}

void FiniteState::InternalProcess() {
  if (_transitions[_state.id].process) {
    _transitions[_state.id].process(_state);
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

void FiniteState::InternalInputTimerCondition() {
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
  if (_transitions[_state.id].predicate) {
    bool state = _transitions[_state.id].predicate(_state.id);
    return state ? True : False;
  }
  return Unspecified;
}

void FiniteState::InternalNextState(TriState state) {
  switch (state) {
    case False:
      this->InternalNextStateAction(_transitions[_state.id].nextF);
      break;
    case True:
      this->InternalNextStateAction(_transitions[_state.id].nextT);
      break;
  }
}


void FiniteState::InternalNextStateAction(const id_t id) {
  if (_state.id == id) return;
  this->InternalExitAction();
  this->InternalEntryAction(id);
  this->InternalDuringAction();
}

void FiniteState::InternalEntryAction(const id_t id) {
  _startTime = micros();
  _timeout = false;
  _state.id = id;
  this->InternalEventHandler(ENTRY);
}

void FiniteState::InternalDuringAction() {
  this->InternalEventHandler(DURING);
  _state.firstScan = true;
}

void FiniteState::InternalExitAction() {
  this->InternalEventHandler(EXIT);
}

void FiniteState::InternalEventHandler(const Action e) {
  if (_eventArgs.action == e) return;
  _eventArgs.id = _state.id;
  _eventArgs.action = e;
  if (_transitions[_state.id].eventHandler) {
    _transitions[_state.id].eventHandler(_eventArgs);
  }
}

const TriState FiniteState::InternalTimer() {
  if (_transitions[_state.id].delayTime < 1) return Unspecified;
  bool timeout = this->InternalTimeout();
  return timeout ? True : False;
}

const bool FiniteState::InternalTimeout() {
  if (_timeout) return true;
  time_t elapsedTime = micros() - _startTime;
  if (elapsedTime < MS2US(_transitions[_state.id].delayTime)) return false;
  _timeout = true;
  return true;
}


const uint8_t FiniteState::InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}