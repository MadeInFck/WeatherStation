#include "DHT.h"
#include <SevSeg.h>
#include <avr/sleep.h>

#define DHTPIN 3 // what pin we're connected to for DHT sensor
#define WAKEPIN 2
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);
SevSeg sevseg;
float temps;
int number;
int val;
int sleepStatus=0;
int previousVal=1;

long time=0;
long debounce=200;

void wakeUpNow()        // here the interrupt is handled after wakeup
{
  digitalWrite(A3,HIGH);
  Serial.println("Reveil en cours!");
  delay(1000);
}

void setup() {
  pinMode(WAKEPIN, INPUT);
  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  pinMode(A2,OUTPUT);
  digitalWrite(A2,LOW);
  pinMode(A3,OUTPUT);
  digitalWrite(A3,HIGH);
  byte numDigits = 4;
  byte digitPins[] = {13, 12, 11, 10};
  byte segmentPins[] = {9, 8, 7, 6, 5, 4, A0, 0};
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);

  Serial.begin(9600);
  Serial.println("DHTxx test!");

  dht.begin();
  temps = millis();
  number = 8888;
  time=millis();
}

void sleepNow()         // here we put the arduino to sleep
{
    Serial.println("Mise en sommeil");
    digitalWrite(A3,LOW);
    delay(1000);
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and 
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings 
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we 
     * choose the according 
     * sleep mode: SLEEP_MODE_PWR_DOWN
     * 
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 

    /* Now it is time to enable an interrupt. We do it here so an 
     * accidentally pushed interrupt button doesn't interrupt 
     * our running program. if you want to be able to run 
     * interrupt code besides the sleep function, place it in 
     * setup() for example.
     * 
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.   
     * 
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */

    attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
    sleepStatus=0;
}

void loop() {
 val=digitalRead(WAKEPIN);
if (val==0 && previousVal==1 && millis()-time>debounce) {
  if (sleepStatus==0) {
    sleepStatus=1;
    sleepNow();
  }
  time=millis();
}
 previousVal=val;
 
  if ((millis() - temps) > 10000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int humid = h;
    int temp = t;
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT");
    } else {
      number = temp * 100 + humid;
      Serial.print("Humidity: ");
      Serial.print(humid);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" *C");
    }
    temps = millis();
  }

  sevseg.setNumber(number, 4);
  sevseg.refreshDisplay();
  sevseg.setBrightness(100);
}
