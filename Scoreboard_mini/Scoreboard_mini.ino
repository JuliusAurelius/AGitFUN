
#define US1_echoPin     7       // Ultra Sonic Echo Pin, Sensor 1
#define US1_trigPin     8       // Ultra Sonic Trigger Pin, Sensor 1
#define US2_echoPin     9       // Ultra Sonic Echo Pin, Sensor 2       // Change!
#define US2_trigPin     10       // Ultra Sonic Trigger Pin, Sensor 2    // Change!

static long ScoreDist[15] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140}; // in cm;

void setup() {
  // =======================(Init Serial)=======================
  Serial.begin(9600);
  Serial.println("Starting...");
  // ====================== Init Display =======================


  // =================== Init Sensors / Fan ====================
  // Ultra Sonic Range
  // Sensor 1
  pinMode(US1_trigPin, OUTPUT);
  pinMode(US1_echoPin, INPUT);
  // Sensor 2
  pinMode(US2_trigPin, OUTPUT);
  pinMode(US2_echoPin, INPUT);
}

void loop() { //############################## LOOP ########################################
  uint16_t scoreT1 = ReadScore(1);
  uint16_t scoreT2 = ReadScore(2);

  SendScore(scoreT1, scoreT2);
  
  delay(1000);
}

void SendScore(uint16_t team1, uint16_t team2) {
  Serial.print("\nScore Team 1: ");
  Serial.print(team1);
  Serial.print("\nScore Team 2: ");
  Serial.print(team2);
}


uint16_t ReadScore(uint8_t team) {
  //________________________________________________________________________________________
  // 1) Read the distance of the required team
  // 2) Call dist2Score to get the score
  //________________________________________________________________________________________
  long distance;

  if (team & 1) {
    distance = readUS(US1_trigPin, US1_echoPin);
  }

  else {
    distance = readUS(US2_trigPin, US2_echoPin);
  }

  return dist2Score(distance);
}


long readUS(int Trig, int Echo) {
  //________________________________________________________________________________________
  // 1) Read the Ultra Sonic Sensor and calculate the distance in cm
  //________________________________________________________________________________________
  int trigPin = Trig;
  int echoPin = Echo;
  long duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  distance = (duration / 2) / 29.1;

  return distance;
}

uint16_t dist2Score(long distance) {
  //________________________________________________________________________________________
  // 1) Check what distances in ScoreDist are shorter than the measurement
  // 2.1) Return 15-i since the measurement is from top down
  // 2.2) Return 0 because the measurement is longer than the entries
  //________________________________________________________________________________________
  uint16_t i = 0;
  uint16_t score = 0;
  while (ScoreDist[i] < distance) {
    if (ScoreDist[i + 1] >= distance) {
      // ScoreDist[i] is smaller equal & ScoreDist[i+1] is greater -> index resemples score
      score = (15 - i);
    }
    i++;
    if (i > 14) {
      break;
    }
  }
  
  if (score < 0) {
    score = 0;
  }
  return score;
}
