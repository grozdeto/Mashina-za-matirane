const int pourDelay = 1500; // how long to pour per shot
const int startButton = 27;
const int turnButton = 8;
int lastTurnButtonState;
const int sensorCount = 4;
const int sensorTriggers[sensorCount] = {22,24,26,28};
const int sensorEchos[sensorCount] = {40,42,44,46};
const int pumpCount = 2;
const int pumps[pumpCount] = {4,5};
int pumpEnables[pumpCount];
int cups[sensorCount];
const int alcoholPerCup[sensorCount] = {0, 0, 1, 1};

void setup() {
  pinMode(startButton, INPUT_PULLUP);
  pinMode(turnButton, INPUT_PULLUP);
  for (int i=0; i<sensorCount; i++) {
    pinMode(sensorTriggers[i], OUTPUT);
    digitalWrite(sensorTriggers[i], LOW);
    pinMode(sensorEchos[i], INPUT);
  }
  for (int i=0; i<pumpCount; i++) {
    pinMode(pumps[i], OUTPUT);
    pumpEnables[i] = 0; // pumps begin disabled
    digitalWrite(pumps[i], HIGH); // the relay works backwards
  }
  for (int i=0; i<sensorCount; i++) {
    cups[i] = 0; // cups begin unshot
  }
  
  
}

void loop() {
  if (digitalRead(startButton) == HIGH) return;
  
  float distances[sensorCount];
  for (int i=0; i<sensorCount; i++) {
    digitalWrite(sensorTriggers[i], LOW);
    delayMicroseconds(2);
    digitalWrite(sensorTriggers[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(sensorTriggers[i], LOW);
    distances[i] = (pulseIn(sensorEchos[i], HIGH)/2)*0.0343;
  }
  
  for (int i=0; i<sensorCount; i++) {
    if (cups[i]) continue; // don't even bother checking already shot cups
    if (distances[i] >= 15 || distances[i] <= 1) continue; // no detection
    pumpEnables[alcoholPerCup[i]] += pourDelay;
  }
  
  for (int i=0; i<pumpCount; i++) {
    if (pumpEnables[i] == 0) continue;
    digitalWrite(pumps[i], LOW);
    delay(pumpEnables[i]);
    digitalWrite(pumps[i], HIGH);
  }
    
  
}

