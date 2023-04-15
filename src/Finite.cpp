/*
  Finite.cpp
  Created:  14-Apr-2023
  Author:   MicroBeaut

  MIT License
  Copyright (c) 2023 MicroBeaut
*/


#include "Finite.h"

Finite::Finite(Transition *transitions, const uint8_t numberOfTransition): size(_numberOfTransitions) {
  _numberOfTransitions = this->InternalLimit(numberOfTransition, STATE_TRANSITION_MIN, STATE_TRANSITION_MAX);
  _transitions = transitions;
  _state.id = 0;
  _prevId = 0;
  _state.action = NONE;
  _prevAction = NONE;
  _initial = false;
  _end = false;
  _state.firstScan = false;
}

void Finite::begin() {
  _initial = true;
  _state.id = 0;
  _prevId = 0;
  _state.action = ENTRY;
  _transitions[_state.id].transition(_state);
  _state.action = NONE;
}

void Finite::state(const uint8_t state) {
  _prevId = _state.id;
  _state.id = this->InternalLimit(state, 0, _numberOfTransitions - 1);
  this->InternalAction();
}

void Finite::execute() {
  if (!_initial) return;
  this->InternalTransition();
}

void Finite::end() {
  if (_state.id == 0) return;
  _end = true;
}

bool Finite::InternalEnd() {
  if (!_end) return false;
  _prevId = _state.id;
  _state.id = 0;
  this->InternalAction();
  _end = false;
  return true;
}

void Finite::InternalTransition() {
  if (_transitions[_state.id].input) {
    if (_transitions[_state.id].input(_state)) {
      if (this->InternalEnd()) return;
      _prevId = _state.id;
      _state.id = _transitions[_state.id].next;
      this->InternalAction();
    } else {
      if (this->InternalEnd()) return;
      _prevId = _state.id;
      _state.id = _transitions[_state.id].current;
      this->InternalAction();
    }

    _transitions[_state.id].transition(_state);

    if (_prevId != _state.id) {
      _state.firstScan = true;
    } else {
      _state.firstScan = false;
    }
  }
  _prevId = _state.id;
}

void Finite::InternalAction() {
  if (_prevId == _state.id) return;
  uint8_t tempId = _state.id;
  _state.id = _prevId;
  _state.action = EXIT;
  _transitions[_prevId].transition(_state);
  _state.id = tempId;
  if (_end) {
    _state.action = END;
    _transitions[_state.id].transition(_state);
    _state.action = NONE;
  }
  _state.action = ENTRY;
  _transitions[_state.id].transition(_state);
  _state.action = NONE;
}

const uint8_t Finite::InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}