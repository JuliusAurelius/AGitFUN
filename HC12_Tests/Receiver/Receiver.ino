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
  delay(300);

  state = not state;
}
