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
const int lights[sensorCount] = {32,33,34,35};

const int calibrationCount = 40;
double highestNormal[sensorCount];
double lowestNormal[sensorCount];
const int extraTolerances[sensorCount] = {0.25,3,0.25,1};

void updateTeam() {
  int newTurnButtonState = digitalRead(turnButton);
  if (newTurnButtonState != lastTurnButtonState) {
    lastTurnButtonState = newTurnButtonState;
    team++;
    if (team == teamCount) team = 0;
    delay(15);
  }
  Serial.print("Team: ");
  Serial.println(team);
  updateLights();
}

double measureSensor(int sensor) {
  digitalWrite(sensorTriggers[sensor], LOW);
  delayMicroseconds(2);
  digitalWrite(sensorTriggers[sensor], HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTriggers[sensor], LOW);
  long long int duration = pulseIn(sensorEchos[sensor], HIGH);
  return duration*0.0343/2;
}

void determineShots() {
  double distances[sensorCount];
  for (int i=0; i<sensorCount; i++) {
    distances[i] = measureSensor(i);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(distances[i]);
  }
  
  for (int i=0; i<sensorCount; i++) {
    if (cups[team][i]) continue; // don't even bother checking already shot cups
    if (distances[i] < 1) continue; // disconnected cable or wrong measurement
    // if ((lowestNormal[i] <= distances[i]) && (distances[i] <= highestNormal[i])) continue; // normal values
    if (distances[i] > 10) continue;
    pumpEnables[alcoholPerCup[i]] += pourDelay[alcoholPerCup[i]];
    cups[team][i] = 1;
  }
}

void updateLights() {
  for (int i=0; i<sensorCount; i++) {
    if (cups[team][i] == 1) {
      digitalWrite(lights[i], LOW);
    } else {
      digitalWrite(lights[i], HIGH);
    }
  }
}

void pumpAction() {
  for (int i=0; i<pumpCount; i++) {
    Serial.print("Pump ");
    Serial.print(i);
    Serial.print(" :");
    Serial.println(pumpEnables[i]);
    if (pumpEnables[i] == 0) continue;
    digitalWrite(pumps[i], LOW);
    //delay(pumpEnables[i]);
    digitalWrite(pumps[i], HIGH);
    pumpEnables[i] = 0;
  }
}

void calibrateSensors() {
  for (int i=0; i<sensorCount; i++) {
    double maximum = 0;
    double minimum = 3000;
    for (int j=0; j<calibrationCount; j++) {
      double measurement = measureSensor(i);
      if (measurement > 2000) continue;
      maximum = max(maximum, measurement);
      minimum = min(minimum, measurement);
    }
    double extra = (maximum-minimum)*extraTolerances[i];
    highestNormal[i] = maximum+extra;
    lowestNormal[i] = minimum-extra;
    Serial.print("Calibrated sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(minimum);
    Serial.print(" - ");
    Serial.println(maximum);
  }
  Serial.println("Measurement complete.");
}

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
  //delay(2000);
  //calibrateSensors();
  updateLights();
}

void loop() {
  if (digitalRead(startButton) == HIGH) {
    for (int i=0; i<sensorCount; i++) {
      for (int j=0; j<teamCount; j++) {
        cups[j][i] = 0;
      }
      digitalWrite(lights[i], LOW);
    }  
    return;
  }
  updateTeam();
  determineShots();
  updateLights();
  pumpAction();
  delay(200);
}

