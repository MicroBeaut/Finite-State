#include "FiniteState.h"

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
void Event(State state);


Transition transitions[] = {
  {Transition1, Input1, 0, 1, Event},
  {Transition2, Input1, 1, 2, Event},
  {Transition3, Input1, 2, 1, Event}
};
const uint8_t numberOftransitions = sizeof(transitions) / sizeof(Transition);

FiniteState finiteStateMachine(transitions, numberOftransitions);

void setup() {
  Serial.begin(115200);
  for (uint8_t index = 0; index < numberOftransitions; index ++) {
    pinMode(pbPins[index], INPUT_PULLUP);
    pinMode(ledPins[index], OUTPUT);
  }
  finiteStateMachine.begin(0);
}

void loop() {
  finiteStateMachine.execute();
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

void Event(State state) {
  Serial.print("State-");
  Serial.print(state.id);
  Serial.print(" Action:= ");
  switch (state.action) {
    case ENTRY:
      Serial.println("ENTRY");
      break;

    case EXIT:
      Serial.println("EXIT");
      break;
  }
}



void InputLED(State state) {
  for (uint8_t index = 0; index < numberOftransitions; index ++) {
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
      Serial.print("\nEXIT -");
      Serial.println(state.id);
      break;
    case ENTRY:
      Serial.print("ENTRY -");
      Serial.println(state.id);
      break;
  }
}