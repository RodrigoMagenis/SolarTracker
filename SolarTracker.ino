#include <Servo.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Wire.h>
#include <Adafruit_INA219.h>


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
            this->servo->attach(pin);
        }

        bool move (int degrees) {
            if (this->setDegrees(degrees)) {
                this->servo->write(this->getDegrees());
                return true;
            } else {
                return false;
            }
        }

        int getDegrees() {
            return this->degrees;
        }
    private:
        Servo *servo;
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

class PowerSensor {
    public:
        PowerSensor() {
            if (!this->ina219->begin()) {
                Serial.println("Failed to find INA219 chip");
                while (1) { delay(10); }
            }

            this->ina219->setCalibration_32V_2A();
            /**
            * powerSensor->setCalibration_32V_2A();
            * powerSensor->setCalibration_32V_1A();
            * powerSensor->setCalibration_16V_400mA();
            *
            */
        }

        float getVoltage() {
            return this->voltage;
        }

        float readVoltage() {
            this->setVoltage(this->ina219->getBusVoltage_V());
            
            /*
                shuntvoltage = ina219.getShuntVoltage_mV();
                busvoltage = ina219.getBusVoltage_V();
                loadvoltage = busvoltage + (shuntvoltage / 1000);

                https://www.youtube.com/watch?v=EihF17x0NXA

                The bus voltage is the total voltage between power and GND. It is the sum of the load voltage and the shunt voltage.
                The load voltage is the voltage going to the load.
                The shunt voltage is the voltage drop across the shunt resistor that is in series with the load.. That is how the sensor measures the current.

                Since current obeys Ohm's Law, the voltage drop across the shunt resistor is proportional to the current flowing through the circuit. And since the resistance of the shunt is known, it is easy to calculate the current.
            */
            return this->getVoltage();
        }

        float getCurrent() {
            return this->current;
        }

        float readCurrent() {
            this->setCurrent(this->ina219->getCurrent_mA());
            return this->getCurrent();
        }

        float getPower() {
            return this->power;
        }

        float readPower() {
            this->setPower(this->ina219->getPower_mW());
            return this->getPower();
        }

        void read() {
            this->readVoltage();
            this->readCurrent();
            this->readPower();
        }

    private:
        float voltage;
        float current;
        float power;
        Adafruit_INA219 *ina219;

        void setVoltage(float value) {
            this->voltage = value;
        }

        void setCurrent(float value) {
            this->current = value;
        }

        void setPower(float value) {
            this->power = value;
        }
};

class Data {
   public:
       unsigned long getTime() {
           return this->time;
       }

       void setPowerData(SensorLDR* obj[]) {
           this->sensors = obj;
       }

   private:
       unsigned long time;
       PowerData* powerData;
       SensorLDR* sensors[];

       void setTime(unsigned long time) {
           this->time = time;
       }
};

SensorLDR *positionSensor1;
SensorLDR *positionSensor2;
SensorLDR *positionSensor3;
SensorLDR *positionSensor4;

PowerSensor *powerSensor;

const int sensorTolerance = 100;
const int pitchAdvance = 5; //graus

ServoMotor *servo1;
ServoMotor *servo2;

ThreadController *threadController;
Thread *threadPositionSensors;
Thread *threadMotorX;
Thread *threadMotorY;
Thread *threadPosition;
Thread *threadPowerSensors;
Thread *threadSaveData;

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
        positionSensor1->read();
        positionSensor2->read();
        positionSensor3->read();
        positionSensor4->read();
    }
}

void moveXAxisEngine() {
    if (compare(positionSensor1, positionSensor2, positionSensor3, positionSensor4) || compare(positionSensor3, positionSensor4, positionSensor1, positionSensor2)) {
        servo1->move(servo1->getDegrees() - pitchAdvance);
    } else if (compare(positionSensor2, positionSensor1, positionSensor3, positionSensor4) || compare(positionSensor4, positionSensor3, positionSensor1, positionSensor2)) {
        servo1->move(servo1->getDegrees() + pitchAdvance);
    } else {
        threadMotorX->enabled = false;
    }
}

void moveYAxisEngine() {
    if (compare(positionSensor1, positionSensor4, positionSensor2, positionSensor3) || compare(positionSensor2, positionSensor3, positionSensor1, positionSensor4)) {
        servo2->move(servo2->getDegrees() - pitchAdvance);
    } else if (compare(positionSensor3, positionSensor2, positionSensor1, positionSensor4) || compare(positionSensor4, positionSensor1, positionSensor2, positionSensor3)) {
        servo2->move(servo2->getDegrees() + pitchAdvance);
    } else {
        threadMotorY->enabled = false;
    }
}

void updatePosition() {
    threadPositionSensors->enabled = true;
    threadMotorX->enabled = true;
    //threadMotorY->enabled = true;
}

void updatePowerValues() {
    powerSensor->read();
}

void saveData() {
//do something
}

void setup() {
    Serial.begin(9600);
    positionSensor1 = new SensorLDR(A0);
    positionSensor2 = new SensorLDR(A6);
    positionSensor3 = new SensorLDR(A4);
    positionSensor4 = new SensorLDR(A2);

    powerSensor = new PowerSensor();

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

    threadPowerSensors->setInterval(300000); // 5 minutes
    threadPowerSensors->onRun(updatePowerValues);

    threadSaveData->setInterval(300000); // 5 minutes
    threadSaveData->onRun(saveData);

    threadController->add(threadPositionSensors);
    threadController->add(threadMotorX);
    //threadController->add(threadMotorY);
    threadController->add(threadPosition);
    //threadController->add(threadPowerSensors);
    //threadController->add(threadSaveData);
}

void loop() {
    threadController->run();
}
