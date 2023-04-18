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
}

void FiniteState::begin(const id_t id) {
  if (this->InternalIDBAD(id)) return;
  _state.id = id;
  _initial = true;
  _state.firstScan = true;
  this->InternalSetAction(ENTRY);
  this->InternalAutoStartTimer(id);
}

void FiniteState::transition(const id_t id) {
  if (this->InternalIDBAD(id)) return;
  this->InternalSetId(id);
}

void FiniteState::execute() {
  if (!_initial) return;
  this->InternalTransition();
}

void FiniteState::delayTime(const id_t id, unsigned long msDelayTime) {
  if (this->InternalIDBAD(id)) return;
  _transitions[id].delayTime = msDelayTime;
}

void FiniteState::manualStart(const id_t id, const bool manualMode) {
  if (this->InternalIDBAD(id)) return;
  if (!(_transitions[id].delayTime > 0)) return;
  _transitions[id].manualStart = manualMode;
}

bool FiniteState::isManualStart(const id_t id) {
  if (this->InternalIDBAD(id)) return false;
  return _transitions[id].manualStart;
}

void FiniteState::startTimer() {
  if (!(_transitions[id].delayTime > 0)) return;
  _startTime = millis();
  _startTimer = true;
}

void FiniteState::stopTimer() {
  if (!_startTimer) return;
  _startTimer = false;
}

bool FiniteState::isStart() {
  return _startTimer;
}

void FiniteState::InternalTransition() {
  bool nextState = false;
  if (_transitions[_state.id].predicateFunc) {
    nextState = _transitions[_state.id].predicateFunc(_state.id);
  }

  bool timeDone = this->InternalTimer(_state.id);
  nextState |= timeDone;

  if (nextState) {
    this->InternalSetId(_transitions[_state.id].next);
  } else {
    this->InternalSetId(_transitions[_state.id].current);
  }

  if (_transitions[_state.id].stateFunc) {
    _transitions[_state.id].stateFunc(_state);
  }

  if (_state.firstScan) _state.firstScan = false;
}

void FiniteState::InternalSetId(const id_t id) {
  if (_state.id == id) return;
  this->InternalSetAction(EXIT);
  _state.id = id;
  _state.firstScan = true;
  this->InternalSetAction(ENTRY);
  this->InternalAutoStartTimer(id);
}

bool FiniteState::InternalIDBAD(const id_t id) {
  if (id < _numberOfTransitions) return false;
  return true;
}

void FiniteState::InternalAutoStartTimer(const id_t id) {
  _elapsedTime = 0UL;
  _startTimer = false;
  if (_transitions[id].manualStart) return;
  if (!(_transitions[id].delayTime > 0)) return;
  _startTime = millis();
  _startTimer = true;
}

bool FiniteState::InternalTimer(const id_t id) {
  if (!_startTimer) return false;
  _elapsedTime = millis() - _startTime;
  if (_elapsedTime < _transitions[id].delayTime) return false;
  _elapsedTime = _transitions[id].delayTime;
  return true;
}

void FiniteState::InternalSetAction(const Events event) {
  if (_eventArgs.event == event) return;
  _eventArgs.id = _state.id;
  _eventArgs.event = event;
  if (_transitions[_state.id].eventFunc) {
    _transitions[_state.id].eventFunc(_eventArgs);
  }
  _eventArgs.event = LOOP;
}

const uint8_t FiniteState::InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}