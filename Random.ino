// Reaktionstrinken for Poolimate
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_PCF8574.h>
#include "SevSeg.h" //Die vorher hinzugefügte Library laden
SevSeg sevseg; //Ein sieben Segment Objekt initialisieren

LiquidCrystal_PCF8574 lcd(0x27);   // set the LCD address to 0x27 for a 16 chars and 2 line displaydisplay


int B_Array [5] = {32, 33, 34, 35, 36};
int R_Array [5] = {27, 28, 29, 30, 31};
int G_Array [5] = {22, 23, 24, 25, 26};
int Hall_Array [5] = {37, 38, 39, 40, 41};

int run_Var;
int penalty;
int startseq;
int n = 5;
int m;
int RGB_idx = 7;
int idx_random;
int loop_change = 1; // Whiles Schleife iteriert == 1 , While Schleife iterriert nicht  == 0;

unsigned long time_start;
unsigned long time_end;
unsigned long final_time;
unsigned long startMillis;
unsigned long currentMillis;
boolean cheat;

/* Initierung der Startsequenz*/
void setup() {

  randomSeed(analogRead(0));  //Random Seed für Zufallszahlen
  Serial.begin(9600);

  for (int i; i < 5; i++) {
    pinMode(R_Array[i], OUTPUT);
    pinMode(G_Array[i], OUTPUT);
    pinMode(B_Array[i], OUTPUT);
    pinMode(Hall_Array[i], INPUT);
  }

  lcd.begin(16, 2); // initialize t

  // Start Sequenz einleiten Wie bei der Formel 1.
  for (int j = 0 ; j < 5; j++) {
    startseq = 5 - j;
    digitalWrite(G_Array[j], HIGH); //R_LED in Reihenfolge starten mit 1000ms delay

    lcd.setBacklight(255);
    lcd.home(); lcd.clear();
    lcd.print( String("GAME START in ") + startseq);
    delay(1000);
  }

  for (int k = 4 ; k > -1; k--) {
    digitalWrite(G_Array[k], LOW); //R_LED auschalten in in Reihenfolge mit 200ms delay

    lcd.setBacklight(255);
    lcd.home(); lcd.clear();
    lcd.print( String("GO ! "));
  }
  time_start = millis(); //Zeitmessung starten
}

void loop() {

  int a[6] = {0, 1, 2, 3, 4, 0}; //Neu


  /* Wird mit jeder loop gechecked, ob Frühstart vorherscht, bevor das grüne Signal vorherscht. Außer in der Start Sequenz.. (noch einprogrammierbar mit if bedingunt und zurücksetzen des Zeitstempels)
     Cheat = true bei: Glas darf erst abgehoben werden, wenn das Grüne Licht aufleuchtet. Bei Frühstart Cheat = true und die besagte Stelle blinkt auf und es passiert nichts.
     Zeitstrafen durch die Delays der Blin Sequenz... Änderbar (weiß nicht wie lang man die setzen soll */
  while ( n > 0) {
    penalty = 50; //delay in ms . insgesamt sind 6 delays in einer For loop interation vorhanden. Heißt für einen Frühstart beträgt die Zeitstrafe 6*penalty in ms
    cheat = false;
  /* RGBs einschalten*/
    for (int i = 0; i < 5 ; i++) {
      if (digitalRead(B_Array[i]) == LOW) {
        digitalWrite(R_Array[i], HIGH);
        digitalWrite(R_Array[RGB_idx], LOW);
        digitalWrite(G_Array[RGB_idx], HIGH);
      }

     /* Nach Frühstartern suchen*/
      if (digitalRead(Hall_Array[i]) == HIGH && digitalRead(G_Array[i]) == LOW && digitalRead(B_Array[i]) == LOW) {
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
      
    }
    /* Kriterien um in der While Schleife zu bleiben oder um in die nächste zu wechseln)*/
    if (loop_change == 1) {
      digitalWrite(G_Array[RGB_idx], LOW);
      idx_random = random(0, n);
      RGB_idx = a[idx_random];
      run_Var = 0;
    }

    /* Feld wird freigegeben, Überprüfung ob die Zeiten*/
    if (digitalRead(Hall_Array[RGB_idx]) == LOW && !cheat) {
      digitalWrite(R_Array[RGB_idx], LOW);
      digitalWrite(G_Array[RGB_idx], HIGH);
    }

    if (digitalRead(G_Array[RGB_idx]) == HIGH && digitalRead(Hall_Array[RGB_idx]) == HIGH && !cheat) {
      run_Var = 1;
      Serial.print("React");
    }

    // run_Var macht noch Probleme
    if (run_Var == 1 && digitalRead(Hall_Array[RGB_idx]) == LOW && !cheat) {
      digitalWrite(G_Array[RGB_idx], LOW);
      digitalWrite(B_Array[RGB_idx], HIGH);
      for (m = idx_random; idx_random < n; idx_random++) {
        a[idx_random] = a[idx_random + 1];
      }
      loop_change = 1;
      n--;
    }
    
    else {
      digitalWrite(B_Array[RGB_idx], LOW);
      digitalWrite(R_Array[RGB_idx], HIGH);
      loop_change = 0;
      time_end = millis();
    }
    
    /*Zeitmessung Pro Feld*/
    final_time = time_end - time_start;
    lcd.setBacklight(255);
    lcd.home(); lcd.clear();
    lcd.print( String("TIME")+ String(":") + final_time + String("")+ String("ms"));
  }
  /*Zeitmessung Gesamt*/
  final_time = time_end - time_start;
  lcd.setBacklight(255);
  lcd.home(); lcd.clear();
  lcd.print( String("TIME")+ String(":") + final_time + String("")+ String("ms"));
}
