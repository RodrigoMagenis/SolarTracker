///* //*********************************************
////* Arduino Current Monitoring
////* with INA219 Module
////* by learnelectronics
////* 30 JAN 2017
////*
////* www.youtube.com/c/learnelectronics
////**********************************************/
//
//
//#include <Wire.h>                                   // Include the Wire lib as we are using I2C
//#include <Adafruit_INA219.h>
//
//Adafruit_INA219 ina219;
//
//void setup()
//{
// 
//   if (!ina219.begin()) {
//    Serial.println("Failed to find INA219 chip");
//    while (1) { delay(10); }
//  }
//}
//
//void loop()
//{
//  shuntvoltage = ina219.getShuntVoltage_mV();
//  busvoltage = ina219.getBusVoltage_V();
//  current_mA = ina219.getCurrent_mA();
//  power_mW = ina219.getPower_mW();
//  loadvoltage = busvoltage + (shuntvoltage / 1000);
//  delay(1000);                                      //wait one second
//}
