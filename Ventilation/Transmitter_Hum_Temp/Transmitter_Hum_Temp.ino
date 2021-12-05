#include <SPI.h>
#include <nRF24L01.h>                                   
#include <RF24.h>
#include "DHT.h"
#define DHTPIN 5
#define DHTTYPE DHT22 
RF24 radio(9,10);
DHT dht(DHTPIN, DHTTYPE);
int data[2];
void setup() {
Serial.begin(9600);
dht.begin();
radio.begin();                                        
delay(2);    
    radio.setChannel(5);                                  
    radio.setDataRate     (RF24_250KBPS);                 
    radio.setPALevel      (RF24_PA_MAX);                 
    radio.openWritingPipe (0xF0F1F2F311LL);
    delay(1000);
}


void loop() {
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  data[0] = h;
  data[1] = t;
  radio.write(&data, sizeof(data));
  delay(5000);
  Serial.print(data[0]);
}
