#include "Finite.h"

#define pbPin1 6
#define pbPin2 4
#define pbPin3 2

#define ledPin1 7
#define ledPin2 5
#define ledPin3 3

uint8_t pbPins[] = {pbPin1, pbPin2, pbPin3};
uint8_t ledPins[] = {ledPin1, ledPin2, ledPin3};


void OutputCondition(State state);

void Transition1(State state);
void Transition2(State state);
void Transition3(State state);
bool Input1(State state);

#define numberOfStateTransitions 3

Transition transitions[] = {
  {Transition1, Input1, 0, 1},
  {Transition2, Input1, 1, 2},
  {Transition3, Input1, 2, 1}
};

Finite finiteStateTransition(transitions, numberOfStateTransitions);

void setup() {
  Serial.begin(115200);
  for (uint8_t index = 0; index < numberOfStateTransitions; index ++) {
    pinMode(pbPins[index], INPUT_PULLUP);
    pinMode(ledPins[index], OUTPUT);
  }
  finiteStateTransition.state(0);
}

void loop() {
  finiteStateTransition.execute();
}

void Transition1(State state) {
  ReportAction(state);
  OutputCondition(state);
}

void Transition2(State state) {
  ReportAction(state);
  OutputCondition(state);
}

void Transition3(State state) {
  ReportAction(state);
  OutputCondition(state);
}

bool Input1(State state) {
  InputLED(state);
  return !digitalRead(pbPins[state.id]);
}


void InputLED(State state) {
  for (uint8_t index = 0; index < numberOfStateTransitions; index ++) {
    digitalWrite(ledPins[index], LOW);
    if ( index == state.id) digitalWrite(ledPins[state.id], HIGH);
  }
  if (state.firstScan) {
    Serial.print("State-");
    Serial.println(state.id);
  }
}

void OutputCondition(State state) {
  if (state.firstScan) {
    Serial.print("Transition-");
    Serial.println(state.id);
  }
}

void ReportAction(State state) {
  switch (state.action) {
    case EXIT:
      Serial.print("EXIT -");
      Serial.println(state.id);
      break;
    case ENTRY:
      Serial.print("ENTRY -");
      Serial.println(state.id);
      break;
  }
}