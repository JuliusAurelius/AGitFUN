// Speed trinken for POOLIMATE
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_PCF8574.h>
#include "SevSeg.h" //Die vorher hinzugefügte Library laden
SevSeg sevseg; //Ein sieben Segment Objekt initialisieren

LiquidCrystal_PCF8574 lcd(0x27);   // set the LCD address to 0x27 for a 16 chars and 2 line display

int B_Array [5] = {32,33,34,35,36};
int R_Array [5] = {27,28,29,30,31};
int G_Array [5] = {22,23,24,25,26};
int Hall_Array [5] = {37,38,39,40,41};

int savePosition_0 = 0;
int savePosition_1;
int savePosition_2;
int savePosition_3;
int savePosition_4;
int endPosition;


int penalty;
int startseq;

unsigned long time_start;
unsigned long time_end;
unsigned long final_time;
unsigned long startMillis; 
unsigned long currentMillis;
boolean cheat;


void setup() {
   for(int i; i<5; i++){
    pinMode(R_Array[i], OUTPUT);
    pinMode(G_Array[i], OUTPUT);
    pinMode(B_Array[i], OUTPUT);
    pinMode(Hall_Array[i], INPUT);
  }

  lcd.begin(16, 2); // initialize the lcd

  // Start Sequenz einleiten Wie bei der Formel 1.
  for(int j = 0 ; j < 5; j++){
   startseq = 5-j;
   
   digitalWrite(G_Array[j], HIGH); //R_LED in Reihenfolge starten mit 1000ms delay     
   lcd.setBacklight(255);
   lcd.home(); lcd.clear();
   lcd.print( String("GAME START in ")+ startseq);
   delay(1000);
   }
   for(int k = 4 ; k > -1; k--){    
    digitalWrite(G_Array[k], LOW); //R_LED auschalten in in Reihenfolge mit 200ms delay
    
    lcd.setBacklight(255);
    lcd.home(); lcd.clear();
    lcd.print( String("GO ! "));    
   }  
   time_start = millis(); //Zeitmessung starten
}

void loop() {
  currentMillis = millis(); //Zeit Variable für Multitasking

     
  /* Wird mit jeder loop gechecked, ob Frühstart vorherscht, bevor das grüne Signal vorherscht. Außer in der Start Sequenz.. (noch einprogrammierbar mit if bedingunt und zurücksetzen des Zeitstempels)
     Cheat = true bei: Glas darf erst abgehoben werden, wenn das Grüne Licht aufleuchtet. Bei Frühstart Cheat = true und die besagte Stelle blinkt auf und es passiert nichts.
     Zeitstrafen durch die Delays der Blin Sequenz... Änderbar (weiß nicht wie lang man die setzen soll */

  penalty = 50; //delay in ms . insgesamt sind 6 delays in einer For loop interation vorhanden. Heißt für einen Frühstart beträgt die Zeitstrafe 6*penalty in ms
  cheat = false;
  for(int i = 0; i < 5 ;i++){
    if(digitalRead(Hall_Array[i]) == HIGH && digitalRead(G_Array[i]) == LOW && digitalRead(B_Array[i]) == LOW){     
      digitalWrite(R_Array[i], LOW); 
      delay(penalty);
      digitalWrite(G_Array[i], HIGH);
      delay(penalty);
      digitalWrite(G_Array[i], LOW); 
      delay(penalty);
      digitalWrite(B_Array[i], HIGH);
      delay(penalty);
      digitalWrite(B_Array[i], LOW);
      delay(penalty);
      digitalWrite(R_Array[i], HIGH);
      delay(penalty);
      time_end = millis();
      cheat = true;
      lcd.setBacklight(255);
      lcd.home(); lcd.clear();
      lcd.print( String("Bad Timing ! "));    
        delay(penalty);
     }
     else if(cheat){
      digitalWrite(R_Array[i], HIGH);
     }
    }

  
  //Case 1
  // Sobald cheat = false kommen wir in die erste Sequenz. Grüne LED geht auf    
    if(digitalRead(Hall_Array[savePosition_0]) == LOW && digitalRead(B_Array[savePosition_0]) == LOW && !cheat){
      digitalWrite(R_Array[savePosition_0], LOW);
      digitalWrite(R_Array[savePosition_0+1], HIGH);
      digitalWrite(R_Array[savePosition_0+2], HIGH);
      digitalWrite(R_Array[savePosition_0+3], HIGH);
      digitalWrite(R_Array[savePosition_0+4], HIGH);
      digitalWrite(G_Array[savePosition_0] , HIGH);
    }
    // Sobald grüne LED leuchtet und das Glas von Hall Sensor entfernt wurde, wird die Laufvariable savePosition_1 auf 1 gesetzt.
    if(digitalRead(G_Array[savePosition_0]) == HIGH && digitalRead(Hall_Array[savePosition_0]) == HIGH && !cheat){
     savePosition_1 = 1; //
    }

    /* Falls Laufvariable auf 1 gesetzt wurde der Sensor wieder ein Magnetfeld detektiert UND in der Zeit wo das Shot Glas vom Sensor weg war kein Frühstart begangen wurde (cheat = true),
       wird das alte Feld auf Blau gesetzt und das neue Feld auf grün freigeschaltet. Falls cheat = true.. muss das geahndete Feld erst wieder zurück positioniert werden. Bevor wir hier in diese Sequenz kommen */
    if(savePosition_1 == 1 && digitalRead(Hall_Array[savePosition_0]) == LOW && digitalRead(B_Array[savePosition_0]) == LOW && !cheat){
      time_end = millis();
      delay(500);
      digitalWrite(G_Array[savePosition_0], LOW);
      digitalWrite(B_Array[savePosition_0], HIGH);
      digitalWrite(R_Array[savePosition_1], LOW);
      digitalWrite(G_Array[savePosition_1], HIGH);
     
    }
      //Case 2
      // analog zu Case 1
      if(savePosition_1 == 1 && digitalRead(G_Array[savePosition_1]) == HIGH && digitalRead(Hall_Array[savePosition_1]) == HIGH && !cheat){
        savePosition_2 = 2;
      }
      
    if(savePosition_2 == 2 && digitalRead(Hall_Array[savePosition_1]) == LOW && digitalRead(B_Array[savePosition_2]) == LOW && !cheat){
      time_end = millis();
      delay(500);
      digitalWrite(G_Array[savePosition_1], LOW);
      digitalWrite(B_Array[savePosition_1], HIGH);
      digitalWrite(R_Array[savePosition_2], LOW);
      digitalWrite(G_Array[savePosition_2], HIGH);
    }

    //Case 3
    // analog zu Case 1
    if(savePosition_2 == 2 && digitalRead(G_Array[savePosition_2]) == HIGH && !cheat && digitalRead(Hall_Array[savePosition_2]) == HIGH){
      savePosition_3 = 3;
    }
    
    if(savePosition_3 == 3 && digitalRead(Hall_Array[savePosition_3]) == LOW && digitalRead(B_Array[savePosition_3]) == LOW && !cheat){
      time_end = millis();
      delay(500);
      digitalWrite(G_Array[savePosition_2], LOW);
      digitalWrite(B_Array[savePosition_2], HIGH);
      digitalWrite(R_Array[savePosition_3], LOW);
      digitalWrite(G_Array[savePosition_3], HIGH);
    }

    //Case 4
    if(savePosition_3 == 3 && digitalRead(G_Array[savePosition_3]) == HIGH && !cheat && digitalRead(Hall_Array[savePosition_3]) == HIGH){
     savePosition_4 = 4;
    }
    
    if(savePosition_4 == 4 && digitalRead(Hall_Array[savePosition_4]) == LOW && digitalRead(B_Array[savePosition_4]) == LOW && !cheat){
     time_end = millis();
     delay(500);
     digitalWrite(G_Array[savePosition_3], LOW);
     digitalWrite(B_Array[savePosition_3], HIGH);
     digitalWrite(R_Array[savePosition_4], LOW);
     digitalWrite(G_Array[savePosition_4], HIGH);
    }

    //Case 5
    if(savePosition_4 == 4 && digitalRead(G_Array[savePosition_4]) == HIGH && !cheat && digitalRead(Hall_Array[savePosition_4]) == HIGH){
      endPosition = 4;
    }
    
    if(endPosition == 4 && digitalRead(Hall_Array[endPosition]) == LOW && !cheat && digitalRead(B_Array[savePosition_4]) == LOW){
      digitalWrite(G_Array[endPosition], LOW);
      digitalWrite(B_Array[endPosition], HIGH);
      time_end = millis();
    }
   
   if(!cheat){
     final_time = time_end - time_start;
     lcd.setBacklight(255);
     lcd.home(); lcd.clear();
     lcd.print( String("TIME")+ String(":") + final_time + String("")+ String("ms"));
   }
 }

 
