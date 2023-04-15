/*
  FiniteState.cpp
  Created:  14-Apr-2023
  Author:   MicroBeaut

  MIT License
  Copyright (c) 2023 MicroBeaut
*/


#include "FiniteState.h"

FiniteState::FiniteState(Transition *transitions, const uint8_t numberOfTransition): size(_numberOfTransitions) {
  _numberOfTransitions = this->InternalLimit(numberOfTransition, STATE_TRANSITION_MIN, STATE_TRANSITION_MAX);
  _transitions = transitions;
  _initial = false;
}

void FiniteState::begin(const uint8_t id) {
  if (!(id < _numberOfTransitions)) return;
  _initial = true;
  _state.id = id;
  _state.firstScan = true;
  this->InternalSetAction(ENTRY);
}

void FiniteState::transition(const uint8_t id) {
  if (!(id < _numberOfTransitions)) return;
  this->InternalSetId(id);
}

void FiniteState::execute() {
  if (!_initial) return;
  this->InternalTransition();
}

void FiniteState::InternalTransition() {
  if (_transitions[_state.id].inputFun) {
    if (_transitions[_state.id].inputFun(_state)) {
      this->InternalSetId(_transitions[_state.id].next);
    } else {
      this->InternalSetId(_transitions[_state.id].current);
    }
  }
  
  if (_transitions[_state.id].transitionFunc) {
    _transitions[_state.id].transitionFunc(_state);
  }

  if (_state.firstScan) _state.firstScan = false;
}

void FiniteState::InternalSetId(uint8_t id) {
  if (_state.id == id) return;
  this->InternalSetAction(EXIT);
  _state.id = id;
  _state.firstScan = true;
  this->InternalSetAction(ENTRY);
}

void FiniteState::InternalSetAction(Actions action) {
  if (_state.action == action) return;
  _state.action = action;
  if (_transitions[_state.id].eventFunc) {
    _transitions[_state.id].eventFunc(_state);
  }
  _state.action = NONE;
}

const uint8_t FiniteState::InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}