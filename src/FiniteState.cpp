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
  _eventArgs.event = PROCESS;
}

void FiniteState::begin(const id_t id) {
  if (BADID(id, _numberOfTransitions)) return;
  _initial = true;
  this->InternalEntry(id);
}

void FiniteState::transition(const id_t id) {
  if (BADID(id, _numberOfTransitions)) return;
  this->InternalExitEntry(id);
}

void FiniteState::execute() {
  if (!_initial) return;
  this->InternalPredicate();
  this->InternalProcess();
}

void FiniteState::InternalPredicate() {
  if (_transitions[_state.id].predicate) {
    bool condition = _transitions[_state.id].predicate(_state.id);
    id_t nextState = condition ? _transitions[_state.id].nextT : _transitions[_state.id].nextF;
    this->InternalExitEntry(nextState);
  }
}

void FiniteState::InternalProcess() {
  if (_transitions[_state.id].process) {
    _transitions[_state.id].process(_state);
  }
  if (_state.firstScan) _state.firstScan = false;
}

void FiniteState::InternalExitEntry(const id_t id) {
  if (_state.id == id) return;
  this->InternalExit();
  this->InternalEntry(id);
}

void FiniteState::InternalEntry(const id_t id) {
  _state.id = id;
  _state.firstScan = true;
  this->InternalEventHandler(ENTRY);
}

void FiniteState::InternalExit() {
  this->InternalEventHandler(EXIT);
}

void FiniteState::InternalEventHandler(const Events e) {
  if (_eventArgs.event == e) return;
  _eventArgs.id = _state.id;
  _eventArgs.event = e;
  if (_transitions[_state.id].eventHandler) {
    _transitions[_state.id].eventHandler(_eventArgs);
  }
  _eventArgs.event = PROCESS;
}

const uint8_t FiniteState::InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}