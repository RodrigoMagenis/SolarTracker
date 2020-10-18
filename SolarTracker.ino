#include <Servo.h>
#include <Thread.h>
#include <ThreadController.h>


class SensorLDR {
    public:
        SensorLDR(int pin) {
            this->setPin(pin);
        }

        int read() {
            this->setValue(analogRead(this->getPin()));
            return this->getValue();
        }

        int getPin() {
            return this->pin;
        }

        int getValue() {
            return this->value;
        }
    private:
        int pin;
        int value;

        void setPin(int pin) {
            this->pin = pin;
        }

        void setValue(int value) {
            this->value = value;
        }
};

class ServoMotor {
    public:
        ServoMotor(int pin) {
            this->servo.attach(pin);
        }

        bool move (int degrees) {
            if (this->setDegrees(degrees)) {
                this->servo.write(this->getDegrees());
                return true;
            } else {
                return false;
            }
        }

        int getDegrees() {
            return this->degrees;
        }
    private:
        Servo servo;
        int pin;
        int degrees = 90;

        bool setDegrees(int degrees) {
            if (degrees >= 0 && degrees <= 180 && degrees != this->degrees) {
                this->degrees = degrees;
                return true;
            } else {
                return false;
            }
        }
};

SensorLDR *sensor1;
SensorLDR *sensor2;
SensorLDR *sensor3;
SensorLDR *sensor4;

const int sensorTolerance = 100;
const int pitchAdvance = 5; //graus

ServoMotor *servo1;
ServoMotor *servo2;

ThreadController *threadController;
Thread *threadPositionSensors;
Thread *threadMotorX;
Thread *threadMotorY;
Thread *threadPosition;

/**
 * Compara valores dos sensores LDRs
 * Retorna verdadeiro caso
 *      o sensor s1 seja o que mais está recebendo incidência luminosa,
 *      a diferença dele para o sensor s2 seja maior que a tolerância
 *
 * s1: Sensor foco
 * s2: Sensor dueto
 * s3: Sensor restante
 * s4: Sensor restante
 **/
bool compare(SensorLDR* s1, SensorLDR* s2, SensorLDR* s3 ,SensorLDR* s4) {
    if(s1->getValue() - s2->getValue() > sensorTolerance && s1->getValue() > s3->getValue() && s1->getValue() > s4->getValue()) {
        return true;
    } else {
        return false;
    }
}

void updatePositionValues() {
    if (!threadMotorX->enabled && !threadMotorY->enabled) {
        threadPositionSensors->enabled = false;
    } else {
        sensor1->read();
        sensor2->read();
        sensor3->read();
        sensor4->read();
    }
}

void moveXAxisEngine() {
    if (compare(sensor1, sensor2, sensor3, sensor4) || compare(sensor3, sensor4, sensor1, sensor2)) {
        servo1->move(servo1->getDegrees() - pitchAdvance);
    } else if (compare(sensor2, sensor1, sensor3, sensor4) || compare(sensor4, sensor3, sensor1, sensor2)) {
        servo1->move(servo1->getDegrees() + pitchAdvance);
    } else {
        threadMotorX->enabled = false;
    }
}

void moveYAxisEngine() {
    if (compare(sensor1, sensor4, sensor2, sensor3) || compare(sensor2, sensor3, sensor1, sensor4)) {
        servo2->move(servo2->getDegrees() - pitchAdvance);
    } else if (compare(sensor3, sensor2, sensor1, sensor4) || compare(sensor4, sensor1, sensor2, sensor3)) {
        servo2->move(servo2->getDegrees() + pitchAdvance);
    } else {
        threadMotorY->enabled = false;
    }
}

void updatePosition() {
    threadPositionSensors->enabled = true;
    threadMotorX->enabled = true;
    threadMotorY->enabled = true;
}

void setup() {
    Serial.begin(9600);
    sensor1 = new SensorLDR(A0);
    sensor2 = new SensorLDR(A6);
    sensor3 = new SensorLDR(A4);
    sensor4 = new SensorLDR(A2);

    servo1 = new ServoMotor(9);
    servo2 = new ServoMotor(11);

    threadPositionSensors->setInterval(500);
    threadPositionSensors->onRun(updatePositionValues);
    threadPositionSensors->enabled = false;

    threadMotorX->setInterval(500);
    threadMotorX->onRun(moveXAxisEngine);
    threadMotorX->enabled = false;

    threadMotorY->setInterval(500);
    threadMotorY->onRun(moveYAxisEngine);
    threadMotorY->enabled = false;

    threadPosition->setInterval(900000); // 15 minutes
    threadPosition->onRun(updatePosition);

    threadController->add(threadPositionSensors);
    threadController->add(threadMotorX);
    threadController->add(threadMotorY);
    threadController->add(threadPosition);
}

void loop() {
    threadController->run();
}