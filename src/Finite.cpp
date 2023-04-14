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
  _state.firstScan = true;
}


void Finite::state(const uint8_t state) {
  _prevId = _state.id;
  _state.id = this->InternalLimit(state, 0, _numberOfTransitions - 1);
}

void Finite::execute() {
  this->InternalTransition();
}

void Finite::InternalTransition() {
  if (_transitions[_state.id].input) {
    if (_transitions[_state.id].input(_state)) {
      _prevId = _state.id;
      _state.id = _transitions[_state.id].next;
      this->InternalAction();
    } else {
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
  _state.action = ENTRY;
  _transitions[_state.id].transition(_state);
  _state.action = NONE;
}

const uint8_t Finite::InternalLimit(const uint8_t value, const uint8_t min, const uint8_t max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}