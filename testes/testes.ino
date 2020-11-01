#include <SPI.h>
#include <SD.h>

File file;
int sampleCount = 0;

void setup() {
    Serial.begin(9600);
    while(!Serial);

    if(!SD.begin(4)) {
      Serial.println("deu ruim");
      while(1);
    } else {
      Serial.println("deu boa");
    }
}

void loop() {
  file = SD.open("log.csv", FILE_WRITE);
  if (file) {
    Serial.println("escrevendo no arquivo");
    file.print(sampleCount);
    file.print(";");
    file.println(rand() % 100 + 1);

    file.close();
    Serial.println("done");
  }
  delay(10000);
}
