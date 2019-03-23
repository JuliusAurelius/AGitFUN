/*
 * Version  0.1
 * Author:  MB
 * Date:    2019.03.15
 */

#include "Adafruit_VL53L0X.h"

#define STATE_GameInit  1
#define STATE_Game      2
#define STATE_TimeOver  3
#define STATE_Break     4

#define DELAY_GameInit  1000     // reduced delay to register button press
#define DELAY_Game      1000
#define DELAY_TimeOver  1000
#define DELAY_Break     1000    // reduced delay to register button press

#define MAX_Score       15
#define MAX_Score_Hist  4

bool      CapOne;              // Winning Team has to score one more point

uint8_t   State;
uint8_t   StateOld;
uint32_t  Delay;
uint64_t  Score;              // Message to Motherchip
uint16_t  ID_Chip;
uint16_t  Score_Hist[2][MAX_Score_Hist];
uint32_t  iHist;
bool      TimeIsOver;
uint16_t  TO_ScoreT1;
uint16_t  TO_ScoreT2;

// Team Variables
//Team 1 is left / Team 2 is right, seen from the display
String    Teams_Current[2];
String    Teams_Next[2];
uint16_t  TeamID_Current[2];
uint16_t  TeamID_Next[2];

// _______ VL53L0X Laser Sensor _______
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  // =======================(Init Serial)=======================
  Serial.begin(9600);
  Serial.println("Starting...");
  // ====================== Init Display ======================= 

  
  // =================== Init Sensors / Fan ====================
  bool SEN_Laser = lox.begin();

  // =================== Init Communication ====================


  // ======================== Init Main ========================
  State = STATE_GameInit;
  Delay = 100;
  Score = 0;
  iHist = 0;
  
  TimeIsOver = false;
  TO_ScoreT1 = 0;
  TO_ScoreT2 = 0;
  
  ID_Chip = 1;
  CapOne  = false;


  SendStartUpReport();

  Serial.println("Started.");
}

void loop() {
  
  ReadInbox();
  
  
  switch (State){
    case STATE_GameInit :
      Serial.println("State: GameInit");
      // Init state
      if(StateOld != STATE_GameInit){
        TimeIsOver = false;

        Teams_Current[0]  = Teams_Next[0];
        Teams_Current[1]  = Teams_Next[1];
        TeamID_Current[0] = TeamID_Next[2];
        TeamID_Current[1] = TeamID_Next[2];
        
        DisplayTeams();
        
        Delay     = DELAY_GameInit;
        StateOld  = STATE_GameInit;
      }
      
      State = GameInit();
      break;
      
    case STATE_Game :
      Serial.println("State: Game");
      // Init state
      if(StateOld != STATE_Game){
        Delay     = DELAY_Game;
        StateOld  = STATE_Game;
      }
      
      State = Game();
      break;
      
    case STATE_TimeOver :
      Serial.println("State: TimeOver");
      // Init state
      if(StateOld != STATE_TimeOver){
        TimeIsOver  = false;
        
        if(iHist>0){
          TO_ScoreT1  = Score_Hist[0][iHist-1];
          TO_ScoreT2  = Score_Hist[1][iHist-1];
        }
        else{
          TO_ScoreT1  = Score_Hist[0][MAX_Score_Hist];
          TO_ScoreT2  = Score_Hist[1][MAX_Score_Hist];
        }
        
        Delay       = DELAY_TimeOver;
        StateOld    = STATE_TimeOver;
      }
      
      
      State = TimeOver(TO_ScoreT1,TO_ScoreT2);
      break;
      
    case STATE_Break :
      Serial.println("State: Break");
      // Init state
      if(StateOld != STATE_Break){
        TimeIsOver  = false;
        Delay       = DELAY_Break;
        StateOld    = STATE_Break;
      }
      
      State = Break();
      break;
  }
  
  delay(Delay);

  
}

// ==================================== State Functions ==================================== 
uint8_t GameInit(){
  //________________________________________________________________________________________
  // 1) Make sure new score is set / old score is not copied
  // 2) Allow teams to switch sides on the board
  //
  // Exit Condition 1: One Team reaches 3 points      Go To: STATE_Game
  // Exit Condition 2: Time is over                   Go To: STATE_TimeOver
  //________________________________________________________________________________________

  uint16_t scoreT1 = ReadScore(1);
  uint16_t scoreT2 = ReadScore(2);

  Score_Hist[1][iHist] = scoreT1;
  Score_Hist[2][iHist] = scoreT2;
  
  iHist++;
  if (iHist==MAX_Score_Hist){
    iHist = 0;
  }

  // Publish score
  SetScore(scoreT1, scoreT2);
  SendScore();
  DisplayTeams(scoreT1, scoreT2);
  
  // Register Button press    ++++++++++++++++ TODO ++++++++++++++++
  if(false){
    SwitchTeams();
    DisplayTeams();
  }

  if(scoreT1 == 3 || scoreT2 == 3){
    return STATE_Game;
  }

  if(TimeIsOver){
    return STATE_TimeOver;
  }

  return STATE_GameInit;
}

uint8_t Game(){
  //________________________________________________________________________________________
  // 1) Keep track of the score
  // 2) Do not allow team-side changes anymore
  // 
  // Exit Condition 1: One team reaches max score     Go To: STATE_Break
  // Exit Condition 2: Time is over                   Go To: STATE_TimeOver
  //________________________________________________________________________________________

  uint16_t scoreT1 = ReadScore(1);
  uint16_t scoreT2 = ReadScore(2);

  Score_Hist[1][iHist] = scoreT1;
  Score_Hist[2][iHist] = scoreT2;
  
  iHist++;
  if (iHist==MAX_Score_Hist){
    iHist = 0;
  }


  // Publish score
  SetScore(scoreT1, scoreT2);
  SendScore();
  DisplayTeams(scoreT1, scoreT2);


  // If one team got to max score wait until the score has been confirmed through 
  // multiple measurements.
  if(scoreT1 == MAX_Score || scoreT2 == MAX_Score){
    bool validWin = ConfirmWin(MAX_Score-1);

    if(validWin){
      return STATE_Break;
    }
  }
  else{
    if(TimeIsOver){
      return STATE_TimeOver;
    }
    return STATE_Game;
  }
}

uint8_t TimeOver(uint16_t TO_ScoreT1, uint16_t TO_ScoreT2){
  //________________________________________________________________________________________
  // 1) Score difference >= 2 || 0 -> No Universe / currently Universe
  // 2) Score difference == 1 -> Possible Universe
  // 3) Check for CapOne Game mode
  //
  // Exit Condition 1: Point was scored, no Universe  Go To: STATE_Break
  //________________________________________________________________________________________

  uint16_t scoreT1 = ReadScore(1);
  uint16_t scoreT2 = ReadScore(2);

  Score_Hist[1][iHist] = scoreT1;
  Score_Hist[2][iHist] = scoreT2;
  
  iHist++;
  if (iHist==MAX_Score_Hist){
    iHist = 0;
  }

  // Publish score
  SetScore(scoreT1, scoreT2);
  SendScore();
  DisplayTeams(scoreT1, scoreT2);

  if(CapOne){
    uint16_t winningBar = (TO_ScoreT1<TO_ScoreT2) ? TO_ScoreT2:TO_ScoreT1;
    bool teamWon = (scoreT1>winningBar) || (scoreT2>winningBar);

    if (teamWon){
      // Confirm win with multiple measurements
      bool validWin = ConfirmWin(winningBar);
      
      if(validWin){
        return STATE_Break;
      }
      else{
        return STATE_TimeOver;
      }
    }
    else{
      return STATE_TimeOver;
    }
  }
  else{
    bool universe = (TO_ScoreT1==TO_ScoreT2);
    bool teamScored = (scoreT1>TO_ScoreT1)||(scoreT2>TO_ScoreT2);

    
    if(teamScored && !universe){
      // One team scored, time is over and no universe
      // Confirm win with multiple measurements
      bool validWin = ConfirmWin(TO_ScoreT1,TO_ScoreT2);

      if(validWin){
        return STATE_Break;
      }
      else{
        return STATE_TimeOver;
      }
      
    }
    else{
      // Either no team scored, or it's universe
      return STATE_TimeOver;
    }
  }
}

uint8_t Break(){
  //________________________________________________________________________________________
  // 1) Wait until GameTime is running (Display Count Down?)
  // 2) Allow Teams to switch sides 
  //________________________________________________________________________________________

  // ++++++++++++++++ TODO ++++++++++++++++
}




// ================================= Additional Functions  ================================= 

void ReadInbox(){
  //________________________________________________________________________________________
  // 1) Set Teams_Next & TeamID_Next
  // 2) Set TimeIsOver (reset happens automatically with state change)
  //________________________________________________________________________________________
  
  // ++++++++++++++++ TODO ++++++++++++++++

  if(false){
    TimeIsOver = true;
  }

  if(false){
    Teams_Next[0] = "ArsLudendi";
    Teams_Next[1] = "ArsBibendi";

    TeamID_Next[0] = 1;     //16bit
    TeamID_Next[1] = 65535; //16bit
  }
}


void SendScore(){
  //________________________________________________________________________________________
  // 1) Send the variable Score 
  //      -> 64bit sorted in [Team ID 1, Score T1, Team ID 2, Score T2], all 16 bit
  //________________________________________________________________________________________
  // ++++++++++++++++ TODO ++++++++++++++++
}

void SendStartUpReport(){
  //________________________________________________________________________________________
  // 1) Send if sensors were started correctly
  //________________________________________________________________________________________
  // ++++++++++++++++ TODO ++++++++++++++++
}

void SetScore(uint16_t ScoreT1, uint16_t ScoreT2){
// [0..15]  Team ID 1
// [16..31] Score Team 1
// [32..47] Team ID 2
// [48..63] Score Team 2

Score = (uint64_t)TeamID_Current[0] | (uint64_t)ScoreT1<<16 | (uint64_t)TeamID_Current[0]<<32 | (uint64_t)ScoreT2<<48;
}


uint16_t ReadScore(uint8_t team){
  if(team & 1){
    // ++++++++++++++++ TODO ++++++++++++++++
  }
  else{
    // ++++++++++++++++ TODO ++++++++++++++++
  }
}

uint16_t ReadScoreWire(uint8_t team){
  // If the raw cable idea is used
  uint16_t di_Pins_T1[] = {1,2,3,4};
  uint16_t di_Pins_T2[] = {5,6,7,8};

  uint16_t wire0;
  uint16_t wire1;
  uint16_t wire2;
  uint16_t wire3;
  
  if(team & 1){
    wire0 = digitalRead(di_Pins_T1[0]);
    wire1 = digitalRead(di_Pins_T1[1]);
    wire2 = digitalRead(di_Pins_T1[2]);
    wire3 = digitalRead(di_Pins_T1[3]);
  }
  else{
    wire0 = digitalRead(di_Pins_T2[0]);
    wire1 = digitalRead(di_Pins_T2[1]);
    wire2 = digitalRead(di_Pins_T2[2]);
    wire3 = digitalRead(di_Pins_T2[3]);
  }

  return wire3<<3 | wire2<<2 | wire1<<1 | wire0;
}

uint16_t ReadScoreLaser(uint8_t team){
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
  if(team & 1){
    // ++++++++++++++++ TODO ++++++++++++++++
  }
  else{
    // ++++++++++++++++ TODO ++++++++++++++++
  }
}

void SwitchTeams(){
  // Names
  String newT2      = Teams_Current[0];
  Teams_Current[0]  = Teams_Current[1];
  Teams_Current[1]  = newT2;

  // IDs
  uint16_t newT2ID  = TeamID_Current[0];
  TeamID_Current[0] = TeamID_Current[0];
  TeamID_Current[0] = newT2ID;
}


void DisplayTeams(){
  // Display Teams with arrow
  // Team 1 is left
  // Team 2 is right

  // ++++++++++++++++ TODO ++++++++++++++++
}

void DisplayTeams(uint16_t score1, uint16_t score2){
  // Display Teams with arrow
  // Team 1 is left
  // Team 2 is right

  // ++++++++++++++++ TODO ++++++++++++++++
}

bool ConfirmWin(uint16_t Border){
  // The point history of one team needs to be 
  // consistently above the given border
  
  bool T1_won = true;
  bool T2_won = true;
      
  for(int i=0;i<MAX_Score_Hist;i++){
    if(Score_Hist[0][i]<=Border){
      T1_won = false;
    }
    if(Score_Hist[1][i]<=Border){
      T2_won = false;
    }
  }

  return (T1_won || T2_won);
}

bool ConfirmWin(uint16_t borderT1, uint16_t borderT2){
  // The point history of one team needs to be 
  // consistently above its own given border
  
  bool T1_won = true;
  bool T2_won = true;
      
  for(int i=0;i<MAX_Score_Hist;i++){
    if(Score_Hist[0][i]<=borderT1){
      T1_won = false;
    }
    if(Score_Hist[1][i]<=borderT2){
      T2_won = false;
    }
  }

  return (T1_won || T2_won);
}
