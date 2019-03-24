//#include <SoftwareSerial.h>
//
//const byte HC12RxdPin = 4;                  // Recieve Pin on HC12
//const byte HC12TxdPin = 5;                  // Transmit Pin on HC12
//
//SoftwareSerial HC12(HC12TxdPin,HC12RxdPin); // Create Software Serial Port
//
//void setup() {
//  //Serial.begin(9600);                       // Open serial port to computer
//  HC12.begin(9600);                         // Open serial port to HC12
//}
//
//void loop() {
//  HC12.write(1);              // Send that data to serial
//  delay(100);
//}

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

  Serial.begin(baud);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial monitor available... OK");

  Serial.print("Serial link available... ");
  HC12.begin(9600);
  if (HC12.isListening()) {
    Serial.println("OK");
  } else {
    Serial.println("NOK");
  }

  //test HC-12
  Serial.print("HC-12 available... ");
  HC12.write("AT+DEFAULT");
  delay(1000);
  while (HC12.available() > 0) {
    Serial.write(HC12.read());
  }
  Serial.println();
  Serial.println("initialization done.");
}

boolean state = false;

void loop() {

  digitalWrite(LED_BUILTIN, state);

  //HC12.println("test123.");


  
  if(HC12.available()>0){                     // If Arduino's HC12 rx buffer has data
    Serial.println("Received: ");              // Send the data to the computer
    while (HC12.available() > 0) {
      Serial.print(char(HC12.read()));
    }
    //HC12.read();
    Serial.println("- Done reading -");
  }
  else{
    Serial.println(HC12.available());
  }
  delay(1500);

  state = not state;
}
