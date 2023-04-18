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
  if (BADID(id, _numberOfTransitions)) return;
  _initial = true;
  _state.id = id;
  _state.firstScan = true;
  this->InternalSetAction(ENTRY);
}

void FiniteState::transition(const id_t id) {
  if (BADID(id, _numberOfTransitions)) return;
  this->InternalSetId(id);
}

void FiniteState::execute() {
  if (!_initial) return;
  this->InternalTransition();
}

void FiniteState::InternalTransition() {
  if (_transitions[_state.id].predicateFunc) {
    if (_transitions[_state.id].predicateFunc(_state.id)) {
      this->InternalSetId(_transitions[_state.id].nextT);
    } else {
      this->InternalSetId(_transitions[_state.id].nextF);
    }
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