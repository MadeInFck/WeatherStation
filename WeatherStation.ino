#include "DHT.h"
#include <SevSeg.h>

#define DHTPIN 2 // what pin we're connected to for DHT sensor

#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);
SevSeg sevseg;
float temps;
int number;

void setup() {

 byte numDigits = 4;
 byte digitPins[] = {13,12,11,10};
 byte segmentPins[] = {9,8,7,6,5,4,3,0};
 sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
 
 Serial.begin(9600); 
 Serial.println("DHTxx test!");

 dht.begin();
 temps=millis();
 number=8888;
}

void loop() {
 if ((millis()-temps) > 10000) {
 float h = dht.readHumidity();
 float t = dht.readTemperature();
 int humid=h;
 int temp=t;
 // check if returns are valid, if they are NaN (not a number) then something went wrong!
 if (isnan(t) || isnan(h)) {
 Serial.println("Failed to read from DHT");
 } else {
 number=temp*100+humid;
 Serial.print("Humidity: "); 
 Serial.print(humid);
 Serial.print(" %\t");
 Serial.print("Temperature: "); 
 Serial.print(temp);
 Serial.println(" *C");
 }
 temps=millis();
 }
 
 sevseg.setNumber(number,4);
 sevseg.refreshDisplay(); 
 sevseg.setBrightness(100);
}
