const int sensor1 = A0; /* Sensor LDR de controle do eixo X */
const int sensor2 = A6; /* Sensor LDR de controle do eixo X */
const int sensor3 = A4; /* Sensor LDR de controle do eixo X */
const int sensor4 = A2; /* Sensor LDR de controle do eixo X */

const int led1 = 5;
const int button1 = 13;

int sensor1Value;
int sensor1Max = 0;
int sensor1Min = 1023;

int sensor2Value;
int sensor2Max = 0;
int sensor2Min = 1023;

int sensor3Value;
int sensor3Max = 0;
int sensor3Min = 1023;

int sensor4Value;
int sensor4Max = 0;
int sensor4Min = 1023;

const int sensorPrecision = 100;

void setup() {
    

    pinMode(led1, OUTPUT);
    pinMode(button1, INPUT);
    Serial.begin(9600);
    // fazer calibração dos sensores caso necessário
   // calibration();
}

void calibration() {
  bool calibrated = false;
  digitalWrite(led1, HIGH);
  while (calibrated == false) {
    while (digitalRead(button1) == HIGH) {
      updateLDRValues();
      if (sensor1Max < sensor1Value) {
        sensor1Max = sensor1Value;
      }
      if (sensor1Min > sensor1Value) {
        sensor1Min = sensor1Value;
      }

      if (sensor2Max < sensor2Value) {
        sensor2Max = sensor2Value;
      }
      if (sensor2Min > sensor2Value) {
        sensor2Min = sensor2Value;
      }

      if (sensor3Max < sensor3Value) {
        sensor3Max = sensor3Value;
      }
      if (sensor3Min > sensor3Value) {
        sensor3Min = sensor3Value;
      }

      if (sensor4Max < sensor4Value) {
        sensor4Max = sensor4Value;
      }
      if (sensor4Min > sensor4Value) {
        sensor4Min = sensor4Value;
      }
      calibrated = true;
      digitalWrite(led1, LOW);
      delay(200);
      digitalWrite(led1, HIGH);
    }
  }
  digitalWrite(led1, LOW);
}

void loop() {
    // aplicar calibração caso ela foi feita - usar função map e constrain
    updateLDRValues();
    Serial.print("sensor1: ");
    Serial.println(sensor1Value);
    Serial.print("sensor2: ");
    Serial.println(sensor2Value);
    Serial.print("sensor3: ");
    Serial.println(sensor3Value);
    Serial.print("sensor4: ");
    Serial.println(sensor4Value);
    Serial.println("==============================");

    if (sensor1Value - sensor2Value > sensorPrecision && sensor1Value > sensor3Value && sensor1Value > sensor4Value) {
        //NO
            Serial.println("cond 1");
    } else if (sensor2Value - sensor1Value > sensorPrecision && sensor2Value > sensor3Value && sensor2Value > sensor4Value) {
        //NE
        Serial.println("cond 2");
    } else if (sensor3Value - sensor4Value > sensorPrecision && sensor3Value > sensor1Value && sensor3Value > sensor2Value) {
        //SO
        Serial.println("cond 3");
    } else if (sensor4Value - sensor3Value > sensorPrecision && sensor4Value > sensor1Value && sensor4Value > sensor2Value) {
        //SE
        Serial.println("cond 4");
    } else {
        // log error
        Serial.println("throw não entrou em nenhuma condição do eixo x");
    }
}

void updateLDRValues() {
  updateLDRValues(false);
}

void updateLDRValues(bool calibration) {
  int s1 = 0;
  int s2 = 0;
  int s3 = 0;
  int s4 = 0;

  for (int i = 0; i < 25; i++) {
    s1 += analogRead(sensor1);
    s2 += analogRead(sensor2);
    s3 += analogRead(sensor3);
    s4 += analogRead(sensor4);
    delay(200);
  }

  if (!calibration) {
    sensor1Value = map(s1 / 25, sensor1Min, sensor1Max, 0, 255);
    sensor2Value = map(s2 / 25, sensor2Min, sensor2Max, 0, 255);
    sensor3Value = map(s3 / 25, sensor3Min, sensor3Max, 0, 255);
    sensor4Value = map(s4 / 25, sensor4Min, sensor4Max, 0, 255);
  }
}
