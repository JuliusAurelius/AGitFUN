#include <SoftwareSerial.h>

#define rxPin 4
#define txPin 5

SoftwareSerial HC12(rxPin, txPin); 
long baud = 9600;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // define pin modes for tx, rx:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  HC12.begin(9600);

  //test HC-12
  
  HC12.write("AT+DEFAULT");
  delay(1000);
}

boolean state = false;

void loop() {

  digitalWrite(LED_BUILTIN, state);

  HC12.println("test123.");
  
  delay(500);

  state = not state;
}
