#include <Servo.h>

/**
 * #####################################
 * Sensores
 * #####################################
 */

const int sensor1 = A0;
int sensor1Value;

const int sensor2 = A6;
int sensor2Value;

const int sensor3 = A4;
int sensor3Value;

const int sensor4 = A2;
int sensor4Value;

const int sensorPrecision = 100;

bool xcalibrated;
bool ycalibrated;

/**
 * #####################################
 * Leds e botões
 * #####################################
 */

const int led1 = 5;
const int button1 = 13;

/**
 * #####################################
 * Motores
 * #####################################
 */

const int pitchAdvance = 5; //graus

const int pinServo1 = 9;
Servo servo1;
int servo1Value = 90;

const int pinServo2 = 11;
Servo servo2;
int servo2Value = 90;

void setup() {
    pinMode(led1, OUTPUT);
    pinMode(button1, INPUT);
    servo1.attach(pinServo1);
    delay(1000);
    servo2.attach(pinServo2);

    Serial.begin(9600);
    // fazer calibração dos sensores caso necessário
}

void loop() {
    xcalibrated = false;
    ycalibrated = false;

    while (xcalibrated == false) {
        updateLDRValues();
        Serial.print("sensor1: ");
        Serial.print(sensor1Value);
        Serial.print("          sensor2: ");
        Serial.print(sensor2Value);
        Serial.print("          sensor3: ");
        Serial.print(sensor3Value);
        Serial.print("          sensor4: ");
        Serial.print(sensor4Value);
        //Serial.println("==============================");
        Serial.print("          ");

        if (sensor1Value - sensor2Value > sensorPrecision && sensor1Value > sensor3Value && sensor1Value > sensor4Value) {
            //NO
                Serial.println("cond 1");
                if (servo1Value >= (0 + pitchAdvance)) {
                servo1Value -= pitchAdvance;
                } else {
                    Serial.println("limit cond 1");
                }
                
        } else if (sensor2Value - sensor1Value > sensorPrecision && sensor2Value > sensor3Value && sensor2Value > sensor4Value) {
            //NE
            Serial.println("cond 2");
            if (servo1Value <= (180 - pitchAdvance)) {
                    servo1Value += pitchAdvance;
            } else {
                Serial.println("limit cond 2");
            }
        } else if (sensor3Value - sensor4Value > sensorPrecision && sensor3Value > sensor1Value && sensor3Value > sensor2Value) {
            //SO
            Serial.println("cond 3");
            if (servo1Value >= (0 + pitchAdvance)) {
                servo1Value -= pitchAdvance;
            } else {
                Serial.println("limit cond 3");
            }
        } else if (sensor4Value - sensor3Value > sensorPrecision && sensor4Value > sensor1Value && sensor4Value > sensor2Value) {
            //SE
            Serial.println("cond 4");
            if (servo1Value <= (180 - pitchAdvance)) {
                    servo1Value += pitchAdvance;
            } else {
                Serial.println("limit cond 4");
            }
        } else {
            // log error
            Serial.println("throw");
            xcalibrated = true;
        }

        servo1.write(servo1Value);
    }

    Serial.println("x calibrado, dormindo zzz");
    delay(2000);
    
    while (ycalibrated == false) {
        updateLDRValues();
        Serial.print("sensor1: ");
        Serial.print(sensor1Value);
        Serial.print("          sensor2: ");
        Serial.print(sensor2Value);
        Serial.print("          sensor3: ");
        Serial.print(sensor3Value);
        Serial.print("          sensor4: ");
        Serial.print(sensor4Value);
        Serial.print("          ");

        if (sensor1Value - sensor4Value > sensorPrecision && sensor1Value > sensor3Value && sensor1Value > sensor2Value) {
            //NO
                Serial.println("cond 1");
                if (servo2Value >= (0 + pitchAdvance)) {
                servo2Value -= pitchAdvance;
                } else {
                    Serial.println("limit cond 1");
                }
                
        } else if (sensor2Value - sensor3Value > sensorPrecision && sensor2Value > sensor1Value && sensor2Value > sensor4Value) {
            //SE
            Serial.println("cond 2");
            if (servo2Value >= (0 + pitchAdvance)) {
                servo2Value -= pitchAdvance;
                } else {
                Serial.println("limit cond 2");
            }
        } else if (sensor3Value - sensor2Value > sensorPrecision && sensor3Value > sensor1Value && sensor3Value > sensor4Value) {
            //SO
            Serial.println("cond 3");
            if (servo2Value <= (180 - pitchAdvance)) {
                    servo2Value += pitchAdvance;
            } else {
                Serial.println("limit cond 3");
            }
        } else if (sensor4Value - sensor1Value > sensorPrecision && sensor4Value > sensor3Value && sensor4Value > sensor2Value) {
            //NE
            Serial.println("cond 4");
            if (servo2Value <= (180 - pitchAdvance)) {
                    servo2Value += pitchAdvance;
            } else {
                Serial.println("limit cond 4");
            }
        } else {
            // log error
            Serial.println("throw");
            ycalibrated = true;
        }

        servo2.write(servo2Value);
    }

    Serial.println("y calibrado, dormindo zzz");
    delay(5000);
}

void updateLDRValues() {
    int s1 = 0;
    int s2 = 0;
    int s3 = 0;
    int s4 = 0;

    for (int i = 0; i < 5; i++) {
        s1 += analogRead(sensor1);
        s2 += analogRead(sensor2);
        s3 += analogRead(sensor3);
        s4 += analogRead(sensor4);
        delay(200);
    }

    sensor1Value = s1 / 5;
    sensor2Value = s2 / 5;
    sensor3Value = s3 / 5;
    sensor4Value = s4 / 5;
}
