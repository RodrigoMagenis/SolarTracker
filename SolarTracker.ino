#include <Servo.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
#include <SD.h>


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
        /**
        * @param pin sets the motor control PWM pin
        * @param upLim sets the upper limit of degrees allowed for the motor. (0 ~ 180)
        * @param lowLim sets the bottom limit of degrees allowed for the engine. (0 ~ 180)
        * @param pitchAdvance sets the number of degrees of motor advance
        **/
        ServoMotor(int pin, int upperLimit = 180, int bottomLimit = 0, int pitchAdvance = 5) {
            this->setPin(pin);
            this->servo = new Servo();
            this->servo->attach(this->getPin());
            this->setUpperLimit(upperLimit);
            this->setBottomLimit(bottomLimit);
            this->setPitchAdvance(pitchAdvance);
            this->move((upperLimit + bottomLimit) / 2);
        }

        bool increaseDegree() {
            return this->move(this->getDegrees() + this->getPitchAdvance());
        }

        bool decreaseDegree() {
            return this->move(this->getDegrees() - this->getPitchAdvance());
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

        int getUpperLimit() {
            return this->upperLimit;
        }

        int getBottomLimit() {
            return this->bottomLimit;
        }

        int getPitchAdvance() {
            return this->pitchAdvance;
        }

        int getPin() {
            return this->pin;
        }
    private:
        Servo *servo;
        int pin;
        int degrees;
        int upperLimit;
        int bottomLimit;
        int pitchAdvance;

        bool setDegrees(int degrees) {
            if (degrees >= this->bottomLimit && degrees <= this->upperLimit && degrees != this->degrees) {
                this->degrees = degrees;
                return true;
            } else {
                return false;
            }
        }

        void setPin(int pin) {
            this->pin = pin;
        }

        void setUpperLimit(int upperLimit) {
            if (upperLimit <= 180) {
                this->upperLimit = upperLimit;
            } else {
                this->upperLimit = 180;
            }
        }

        void setBottomLimit(int bottomLimit) {
            if (bottomLimit >= 0) {
                this->bottomLimit = bottomLimit;
            } else {
                this->bottomLimit = 0;
            }
        }

        void setPitchAdvance(int pitchAdvance) {
            if (pitchAdvance >= 1 && pitchAdvance <= 180) {
                this->pitchAdvance = pitchAdvance;
            } else {
                this->pitchAdvance = 1;
            }
        }
};

class PowerSensor {
    public:
        PowerSensor() {
            if (!this->ina219->begin()) {
                Serial.println("Failed to find INA219 chip");
                while (1) { delay(1000); }
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
        Data() {
            this->setTime(millis());
        }
        void setPowerSensor(PowerSensor *powerSensor) {
           this->powerSensor = powerSensor;
        }
       
        void setLDRs(SensorLDR *ldr1, SensorLDR *ldr2, SensorLDR *ldr3, SensorLDR *ldr4) {
            this->ldr1 = ldr1;
            this->ldr2 = ldr2;
            this->ldr3 = ldr3;
            this->ldr4 = ldr4;
        }

        void setServos(ServoMotor *servo1, ServoMotor *servo2) {
            this->servo1 = servo1;
            this->servo2 = servo2;
        }

        void persistData() {
            this->file = SD.open("log.csv", FILE_WRITE);
            if (this->file) {
                this->file.print(this->getTime());
                this->addSeparator();
                // this->file->print(this->powerSensor->getVoltage());
                // this->addSeparator();
                // this->file->print(this->powerSensor->getCurrent()):
                // this->addSeparator();
                // this->file->print(this->powerSensor->getPower());
                // this->addSeparator();
                this->file.print(this->ldr1->getValue());
                this->addSeparator();
                this->file.print(this->ldr2->getValue());
                this->addSeparator();
                this->file.print(this->ldr3->getValue());
                this->addSeparator();
                this->file.print(this->ldr4->getValue());
                this->addSeparator();
                this->file.print(this->servo1->getDegrees());
                this->addSeparator();
                this->file.println(this->servo2->getDegrees());
                this->file.close();
                Serial.println("opabier");
            } else {
                Serial.println("Failed to open CSV file");
            }
        }

        unsigned long getTime() {
            return this->time;
        }

    private:
        unsigned long time;
        PowerSensor *powerSensor;
        SensorLDR *ldr1;
        SensorLDR *ldr2;
        SensorLDR *ldr3;
        SensorLDR *ldr4;
        ServoMotor *servo1;
        ServoMotor *servo2;
        File file;

        void setTime(unsigned long time) {
            this->time = time;
        }

        void addSeparator() {
            this->file.print(";");
        }
};

SensorLDR *positionSensor1;
SensorLDR *positionSensor2;
SensorLDR *positionSensor3;
SensorLDR *positionSensor4;

PowerSensor *powerSensor;

const int sensorTolerance = 80;

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
        Serial.print("s1: ");
        Serial.print(positionSensor1->getValue());
        Serial.print("   s2: ");
        Serial.print(positionSensor2->getValue());
        Serial.print("   s3: ");
        Serial.print(positionSensor3->getValue());
        Serial.print("   s4: ");
        Serial.print(positionSensor4->getValue());
        Serial.print("   SERVOX: ");
        Serial.print(servo1->getDegrees());
        Serial.print("   SERVOY: ");
        Serial.println(servo2->getDegrees());
    }
}

void moveXAxisEngine() {
    if (compare(positionSensor1, positionSensor2, positionSensor3, positionSensor4) || compare(positionSensor3, positionSensor4, positionSensor1, positionSensor2)) {
        servo1->decreaseDegree();
    } else if (compare(positionSensor2, positionSensor1, positionSensor3, positionSensor4) || compare(positionSensor4, positionSensor3, positionSensor1, positionSensor2)) {
        servo1->increaseDegree();
    } else {
        threadMotorX->enabled = false;
    }
}

void moveYAxisEngine() {
    if (compare(positionSensor1, positionSensor4, positionSensor2, positionSensor3) || compare(positionSensor2, positionSensor3, positionSensor1, positionSensor4)) {
        servo2->decreaseDegree();
    } else if (compare(positionSensor3, positionSensor2, positionSensor1, positionSensor4) || compare(positionSensor4, positionSensor1, positionSensor2, positionSensor3)) {
        if (!servo2->increaseDegree()) {
            servo2->move(90);
            servo1->move(90);
        }
    } else {
        threadMotorY->enabled = false;
    }
}

void updatePosition() {
    Serial.println("updatePosition");
    threadPositionSensors->enabled = true;
    threadMotorX->enabled = true;
    threadMotorY->enabled = true;
}

void updatePowerValues() {
    Serial.println("updatePowerValues");
    // powerSensor->read();
}

void saveData() {
    Data *data = new Data();
    //data->setPowerSensor(this->powerSensor);
    data->setLDRs(positionSensor1, positionSensor2, positionSensor3, positionSensor4);
    data->setServos(servo1, servo2);
    data->persistData();

// Arduino Mega
// SS 53
// MOSI 51
// MISO 50
// SCK 52
}

void setup() {
    Serial.begin(9600);
    while(!Serial);

    positionSensor1 = new SensorLDR(A2);
    positionSensor2 = new SensorLDR(A4);
    positionSensor3 = new SensorLDR(A0);
    positionSensor4 = new SensorLDR(A6);

    servo1 = new ServoMotor(11);
    servo2 = new ServoMotor(9, 110, 50);

    //powerSensor = new PowerSensor(); Só funciona com o sensor conectado

    if(!SD.begin(4)) {
        Serial.println("Failed to find SD chip");
        while(1) {delay(1000);};
    }

    threadPositionSensors = new Thread();
    threadPositionSensors->setInterval(1000);
    threadPositionSensors->onRun(updatePositionValues);

    threadMotorX = new Thread();
    threadMotorX->setInterval(1500);
    threadMotorX->onRun(moveXAxisEngine);

    threadMotorY = new Thread();
    threadMotorY->setInterval(1500);
    threadMotorY->onRun(moveYAxisEngine);

    threadPosition = new Thread();
    threadPosition->setInterval(5000); // 15 minutes
    threadPosition->onRun(updatePosition);

    threadPowerSensors = new Thread();
    threadPowerSensors->setInterval(1000); // 5 minutes
    threadPowerSensors->onRun(updatePowerValues);

    threadSaveData = new Thread();
    threadSaveData->setInterval(1000); // 5 minutes
    threadSaveData->onRun(saveData);

    threadController = new ThreadController();
    threadController->add(threadPositionSensors);
    threadController->add(threadMotorX);
    threadController->add(threadMotorY);
    threadController->add(threadPosition);
    // threadController->add(threadPowerSensors);
    threadController->add(threadSaveData);
}

void loop() {
    threadController->run();
}
