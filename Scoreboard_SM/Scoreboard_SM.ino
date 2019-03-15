/*
 * Version  0.1
 * Author:  MB
 * Date:    2019.03.15
 */
 
#define STATE_GameInit  1
#define STATE_Game      2
#define STATE_TimeOver  3
#define STATE_Break     4

uint8_t   State;
uint32_t  Delay;
uint64_t  Score;
uint16_t  ID_Chip;


// Team Variables
String    Teams_Current[2];
String    Teams_Next[2];
uint16_t  TeamID_Current[2];
uint16_t  TeamID_Next[2];

void setup() {
  // ======================= Init Serial =======================
  
  // ====================== Init Display ======================= 
  
  // ====================== Init Sensors =======================

  // =================== Init Communication ====================

  // ======================== Init Main ========================

  
  State = STATE_GameInit;
  Delay = 100;
  Score = 0;

  ID_Chip = 1;
}

void loop() {
  
  ReadInbox();
  
  switch (State){
    case STATE_GameInit :
      State = GameInit();
      break;
      
    case STATE_Game :
      State = Game();
      break;
      
    case STATE_TimeOver :
      State = TimeOver();
      break;
      
    case STATE_Break :
      State = Break();
      break;
  }

  SendScore();
  
  delay(Delay);
}

// ==================================== State Functions ==================================== 
uint8_t GameInit(){
  // 1) Make sure new score is set / old score is not copied
  // 2) Allow teams to switch sides on the board

  // Exit Condition 1: One Team reaches 3 points      Go To: STATE_Game
  // Exit Condition 2: Time is over                   Go To: STATE_TimeOver
}

uint8_t Game(){
  // 1) Keep track of the score
  // 2) Do not allow team-side changes anymore

  // Exit Condition 1: One team reaches 15 points     Go To: STATE_Break
  // Exit Condition 2: Time is over                   Go To: STATE_TimeOver
}

uint8_t TimeOver(){
  // 1) Score difference >= 2 || 0 -> No Universe / currently Universe
  // 2) Score difference == 1 -> Possible Universe

  // Exit Condition 1: Point was scored, no Universe  Go To: STATE_Break
}

uint8_t Break(){
  // 1) Wait until GameTime is running (Display Count Down?)
  // 2) Allow Teams to switch sides 
}

// ================================= Additional Functions  ================================= 

void ReadInbox(){
  
}

void SendScore(){
  
}

void SetScore(uint16_t ScoreT1, uint16_t ScoreT2){
// [0..15]  Team ID 1
// [16..31] Score Team 1
// [32..47] Team ID 2
// [48..63] Score Team 2

Score = TeamID_Current[0] | (uint64_t)ScoreT1<<16 | (uint64_t)TeamID_Current[0]<<32 | (uint64_t)ScoreT2<<48;
}
