const int teamCount = 2;
const int pourDelay[2] = {10000,5000}; // how long to pour per shot
const int startButton = 27;
const int turnButton = 8;
int lastTurnButtonState;
const int sensorCount = 4;
const int sensorTriggers[sensorCount] = {22,24,26,28};
const int sensorEchos[sensorCount] = {40,42,44,46};
const int pumpCount = 2;
const int pumps[pumpCount] = {4,5};
int pumpEnables[pumpCount];
int cups[teamCount][sensorCount];
const int alcoholPerCup[sensorCount] = {1,0,1,0};
int team = 0;

char serialBuffer[150];

void setup() {
  Serial.begin(19200);
  pinMode(startButton, INPUT_PULLUP);
  pinMode(turnButton, INPUT_PULLUP);
  lastTurnButtonState = digitalRead(turnButton);
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
  for (int t=0; t<teamCount; t++) {
    for (int i=0; i<sensorCount; i++) {
      cups[t][i] = 0; // cups begin unshot
    }
  }
  
}

void loop() {
  if (digitalRead(startButton) == HIGH) return;
  
  int newTurnButtonState = digitalRead(turnButton);
  if (newTurnButtonState != lastTurnButtonState) {
    lastTurnButtonState = newTurnButtonState;
    team++;
    if (team == teamCount) team = 0;
    delay(15);
  }
  Serial.print("Team: ");
  Serial.println(team);
  
  float distances[sensorCount];
  for (int i=0; i<sensorCount; i++) {
    digitalWrite(sensorTriggers[i], LOW);
    delayMicroseconds(2);
    digitalWrite(sensorTriggers[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(sensorTriggers[i], LOW);
    float duration = pulseIn(sensorEchos[i], HIGH);
    distances[i] = (duration/2)*0.0343;
    
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(distances[i]);
  }
  
  for (int i=0; i<sensorCount; i++) {
    if (cups[team][i]) continue; // don't even bother checking already shot cups
    if (distances[i] >= 15 || distances[i] <= 1) continue; // no detection
    pumpEnables[alcoholPerCup[i]] += pourDelay[alcoholPerCup[i]];
    cups[team][i] = 1;
  }
  
  for (int i=0; i<pumpCount; i++) {
    Serial.print("Pump ");
    Serial.print(i);
    Serial.print(" :");
    Serial.println(pumpEnables[i]);
    if (pumpEnables[i] == 0) continue;
    digitalWrite(pumps[i], LOW);
    delay(pumpEnables[i]);
    digitalWrite(pumps[i], HIGH);
    pumpEnables[i] = 0;
  }
  
}

