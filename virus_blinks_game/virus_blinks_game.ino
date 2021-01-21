/*
 Virus
 Dev and design by Nick
*/

Color tileColors[4] = { ORANGE, YELLOW, CYAN, GREEN };
enum tileColorBytes { TILE_ORANGE, TILE_YELLOW, TILE_CYAN, TILE_GREEN };
byte tileColorByte = TILE_ORANGE;

enum signalStates {INITIALIZE, GAME, VIRUS, TILE_FIRE, TILE_CASCADE, WIN, LOSE};
byte signalState = INITIALIZE;

enum messageType {MSG_RESET, MSG_MATCHING, MSG_HIT, MSG_SCORE};
enum resetStates {RESET_INERT, RESET_INITIALIZE, RESET_WIN, RESET_LOSE};

int pathToMaster = -1;
int pathToLaser = -1;

bool master = false;
bool masterSetup = false;

Color virusColors[3] = { RED, WHITE, BLUE };
Timer virusTimer;
int currentHealth = 0;
#define VIRUS_MAX_HEALTH 18
#define VIRUS_HEALTH_STEPS 6
#define VIRUS_TIMER_INTERVALS 15000

int score = 0;
Timer scoringTimer;
bool hasProcessedScoring = true;
#define SCORING_TIMER_DURATION 1000

Timer resetTimer;
#define RESET_DURATION 300
Timer refreshTimer;
bool hasProcessedRefresh = true;
bool hasProcessedHit = false;
#define REFRESH_DURATION 400

bool isFireSource = false;
bool hasProcessedInitialize = false;
Timer resetPullTimer;
#define RESET_PULL_TIMER_PULSE 1000

void resetToDefaults(){
  isFireSource = false;
  hasProcessedHit = false;
  hasProcessedRefresh = true;
  hasProcessedScoring = true;
  score = 0;
  currentHealth = 0;
  masterSetup = false;
  master = false;
  pathToMaster = -1;
  pathToLaser = -1;
}

void setup() {
  randomize();
  initializeTile(true);
}

void loop() {
  switch(signalState) {
    case INITIALIZE:
      initializeLoop();
      break;
    case GAME:
      gameLoop();
      break;
    case VIRUS:
      virusLoop();
      break;
    case TILE_FIRE:
      tileFireLoop();
      break;
    case TILE_CASCADE:
      tileCascadeLoop();
      break;
    case WIN:
      resetLoop(WHITE);
      break;
    case LOSE:
      resetLoop(RED);
      break;
  }
}

void resetLoop(Color pulseColor) {

  if(!hasProcessedInitialize) {
    //console.log("RESET TO DEFAULTS CALLED");
    resetToDefaults();
    resetPullTimer.set(RESET_PULL_TIMER_PULSE);
    hasProcessedInitialize = true;
  }

  int pulseInterval = RESET_PULL_TIMER_PULSE;
  byte displayBrightness = sin8_C(map(resetPullTimer.getRemaining(), 0, pulseInterval, 0, 255));
  setColor(dim(pulseColor, displayBrightness));
  if(resetPullTimer.isExpired()) {
    resetPullTimer.set(RESET_PULL_TIMER_PULSE);
  }

  if(buttonDoubleClicked()) {
    hasProcessedRefresh = false;
    master = true;
    masterSetup = false;
    isFireSource = false;
    signalState = VIRUS;
    //console.log("manual reset caused by double click logic in reset loop");
    resetGame(RESET_INITIALIZE);
    //console.clear();
  }

  FOREACH_FACE(f){
    if(!isValueReceivedOnFaceExpired(f)){
      if(getMessageType(getLastValueReceivedOnFace(f)) == MSG_RESET && didValueOnFaceChange(f)){
        
        switch(getMessageValue(getLastValueReceivedOnFace(f))){
          case RESET_INITIALIZE:
              pathToMaster = f;
              hasProcessedRefresh = false;
              hasProcessedHit = true;
              signalState = GAME;
              master = false;
              isFireSource = false;
              score = 0;
              //setValueSentOnAllFaces(setMessage(MSG_RESET, RESET_INITIALIZE));
              //console.log("manual reset caused by reset loop faces");
              resetGame(RESET_INITIALIZE);
            break;
          case RESET_WIN:
            resetGame(RESET_WIN);
            break;
          case RESET_LOSE:
            resetGame(RESET_LOSE);
            break;
        }

      }
    }
  }
}

void initializeLoop(){
  if(buttonDoubleClicked()) {
    hasProcessedRefresh = false;
    master = true;
    masterSetup = false;
    isFireSource = false;
    signalState = VIRUS;
    //console.log("manual reset caused by double click logic in initialize loop");
    resetGame(RESET_INITIALIZE);
    //console.clear();
  }
  
  FOREACH_FACE(f){
    if(!isValueReceivedOnFaceExpired(f)){
      if(getMessageType(getLastValueReceivedOnFace(f)) == MSG_RESET && didValueOnFaceChange(f)){
        if(pathToMaster == -1) {
          pathToMaster = f;
        }
        hasProcessedRefresh = false;
        hasProcessedHit = true;
        signalState = GAME;
        master = false;
        isFireSource = false;
        score = 0;
        //setValueSentOnAllFaces(setMessage(MSG_RESET, RESET_INITIALIZE));
        //console.log("manual reset caused by initialize loop faces");
        resetGame(RESET_INITIALIZE);
      }
    }
  }
}

void virusLoop(){
  
  if(!masterSetup) {
    currentHealth = 13;
    masterSetup = true;
    virusTimer.set(VIRUS_TIMER_INTERVALS);
  }
  
  FOREACH_FACE(f){
    if(!isValueReceivedOnFaceExpired(f)){
      if(getMessageType(getLastValueReceivedOnFace(f)) == MSG_SCORE && didValueOnFaceChange(f)){
        int hitTotal = getMessageValue(getLastValueReceivedOnFace(f));
        //console.log("hitting virus for " + hitTotal);

        currentHealth -= getHitBasedOnRules(hitTotal);
        //console.log("current health " + currentHealth + " out of " + VIRUS_MAX_HEALTH);

        if(currentHealth <= 0){
          //console.log("YOU HAVE WON");
          signalState = GAME;
          resetGame(RESET_WIN);
        }

        if(currentHealth >= VIRUS_MAX_HEALTH){
          //console.log("YOU HAVE LOST TO YOUR OWN DOING");
          signalState = GAME;
          resetGame(RESET_LOSE);
        }
      }

      
    }
  }

  renderVirus();
  virusTimerProcess();
}

void renderVirus(){
  int currentTier = int(currentHealth / VIRUS_HEALTH_STEPS);
  int pips = currentHealth % VIRUS_HEALTH_STEPS;
  Color thisColor = virusColors[currentTier];
  Color previousColor = OFF;

  if(currentTier == 0){
    previousColor = OFF;
  } else {
    previousColor = virusColors[currentTier - 1];
  }

  int pulseInterval = VIRUS_TIMER_INTERVALS;

  byte displayBrightness = sin8_C(map(virusTimer.getRemaining(), 0, pulseInterval, 0, 255));

  setColor(dim(previousColor, displayBrightness));
  for(int i = 0; i < pips; ++i){
    setColorOnFace(dim(thisColor, displayBrightness), i);
  }
}

void gameLoop(){
  setColor(tileColors[tileColorByte]);
  if(buttonSingleClicked()) {
    signalState = TILE_FIRE;
    isFireSource = true;
    pathToLaser = -1;
    
    //console.clear();
    scoringTimer.set(SCORING_TIMER_DURATION);
  }
  
  refreshTimerProcess();
  scoreTimerProcess();
  resetTimerProcess();
  
  laserFaceCheck();

  //if(pathToMaster >= 0) {
  //  setColorOnFace(WHITE, pathToMaster);
  //}
}

void tileFireLoop(){
  score = 1;
  isFireSource = true;
  propagateChoice();
}

void tileCascadeLoop(){
  score = 1;
  isFireSource = false;
  propagateChoice();
}

void virusTimerProcess() {
  if(virusTimer.isExpired() && currentHealth < VIRUS_MAX_HEALTH && currentHealth > 0) {
    //console.log("VIRUS TIMER");

    ++currentHealth;

    if(currentHealth >= VIRUS_MAX_HEALTH){
      //console.log("VIRUS HAS WON");
      signalState = GAME;
      resetGame(RESET_LOSE);
      return;
    }

    virusTimer.set(VIRUS_TIMER_INTERVALS);
  }
}

void refreshTimerProcess() {
  // After a reset
  if(refreshTimer.isExpired() && !hasProcessedRefresh){
    setValueSentOnAllFaces(0);

    hasProcessedScoring = false;

    if(!hasProcessedHit){
      if(pathToLaser >= 0 && !hasProcessedHit){
        sendHit(score);
        resetTimer.set(RESET_DURATION);
      } 

      hasProcessedHit = true;
    }

    initializeTile(false);
  }  
}

void resetTimerProcess() {
  if(resetTimer.isExpired() && pathToLaser != -1 && hasProcessedRefresh) {
    pathToLaser = -1;
  }
}

void scoreTimerProcess() {
  if(scoringTimer.isExpired() && !hasProcessedScoring && isFireSource){
    setValueSentOnAllFaces(0);
    //console.log("ENDING scoring timer with " + score);
    
    hasProcessedScoring = true;
    isFireSource = false;

    sendScore(score);
    score = 0;
  }
}

void sendHit(int hitScore){
  //console.log("sending a hit to " + pathToLaser + " with " + hitScore);
  setValueSentOnAllFaces(0);

  if(hitScore> 7){
    //console.log("clamping score to 7");
    setValueSentOnFace(setMessage(MSG_HIT, 7), pathToLaser);
  } else {
    setValueSentOnFace(setMessage(MSG_HIT, hitScore), pathToLaser);
  }
  score = 0;

  // Reset this
  //pathToLaser = -1;
}

void sendScore(int hitScore){
  if(hitScore > 7) {
    //console.log("clamping score to 7");
    hitScore = 7;
  }
  //console.log("sending a SCORE to " + pathToMaster + " with " + hitScore);
  setValueSentOnAllFaces(0);
  setValueSentOnFace(setMessage(MSG_SCORE, hitScore), pathToMaster);
}

void laserFaceCheck() {

  int hitCounter = 0;

  FOREACH_FACE(f){
    if(!isValueReceivedOnFaceExpired(f) && didValueOnFaceChange(f)){
      switch(getMessageType(getLastValueReceivedOnFace(f))) {
        case MSG_MATCHING:
          if( getMessageValue(getLastValueReceivedOnFace(f)) == tileColorByte && !isFireSource && pathToLaser == -1){
            signalState = TILE_CASCADE;
            pathToLaser = f;
          }
          break;
        case MSG_HIT:
          hitCounter += getMessageValue(getLastValueReceivedOnFace(f));
          break;
        case MSG_SCORE:
          int hitTotal = getMessageValue(getLastValueReceivedOnFace(f));
          //console.log("FORWARDING SCORE of " + hitTotal);
          sendScore(hitTotal);
          break;
        case MSG_RESET:
          byte type = getMessageValue(getLastValueReceivedOnFace(f));
          switch(type) {
            case RESET_INITIALIZE:
              //console.log("reset INITIALIZE detected");
              signalState = INITIALIZE;
              break;
            case RESET_WIN:
              //console.log("reset WIN detected");
              signalState = WIN;
              break;
            case RESET_LOSE:
              //console.log("reset LOSE detected");
              signalState = LOSE;
              break;
          }
          if(type > RESET_INERT) {
            resetGame(type);
          }
          break;
      }
    }
  }

  if(hitCounter > 0){
    if(pathToLaser == -1){
      //console.log("a hit got home for " + hitCounter + " and a total of " + (hitCounter + score));
      score += hitCounter;
    } else {
      //console.log("process the hit here for " + hitCounter);
      sendHit(hitCounter + score);
    }
  }
}

void resetGame(byte resetType){
  //console.log("reset manually called with " + resetType);
  setValueSentOnAllFaces(setMessage(MSG_RESET, resetType));

  if(resetType > RESET_INITIALIZE) {
    //hasProcessedInitialize = false;
  }
}

int getHitBasedOnRules(int hitAmount){
  if(hitAmount < 2){
    return -1;
  }

  if(hitAmount == 2){
    return 0;
  }

  if(hitAmount < 6) {
    return 1;
  }

  return 2;
}

void initializeTile(bool clear){
  if(clear){
    setColor(WHITE);
  } else {
    tileColorByte = random(COUNT_OF(tileColors)-1);
    setColor(tileColors[tileColorByte]);
    hasProcessedRefresh = true;
  }
}

void propagateChoice(){
  hasProcessedRefresh = false;
  hasProcessedHit = false;
  signalState = GAME;
  setColor(OFF);
  refreshTimer.set(REFRESH_DURATION);
  setValueSentOnAllFaces(setMessage(MSG_MATCHING, tileColorByte));
}

byte getMessageValue(byte data) {
  return (data & 7);
}

byte getMessageType(byte data) {
  return ((data >> 3) & 7);
}

byte setMessage(byte msg, byte value){
  return (msg << 3) | value;
}
