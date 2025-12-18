#include "libs/services.h"
#include "libs/firebaseAccess.h"



///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  

  // pins definitions
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, HIGH);
  pinMode(WIPEBUTTON_PIN, INPUT);

  //Init RFID
  initRFIDModule();

  //Init DY50
  initFingerSensor();

  //Init Display
  setupLCD();
  showStartingSystemMessage();

  //Init Serial 
  Serial.begin(9600);  // Initialize serial communications with PC   

  // Check if Master Card is defined
  checkMasterDefinition();

  //init firebase connection
  setupFirebase();

  //init time for logging timestamp
  setupTime();

  // Show waiting access message
  showWaitingAccessMessage();
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {

  do { // Trying to scan a card
    successRead = tryScanAccessMethod(&fingerExists, &fingerIdRead);  // sets successRead to 1 when we get read from reader otherwise 0
  } while (!successRead);   //the program will not go further while you are not getting a successful read
  


  if ( isMaster(readCard)) {    // If scanned card's ID matches Master Card's ID - enter program mode
    soundEntryMasterMode();
    showEntryMasterModeMessage();
    Serial.println("Hello Master - Entered Program Mode");
    uint8_t numCards = getNumCards();   // Read the first Byte of EEPROM that
    Serial.print("I have ");     // stores the number of ID's in EEPROM
    Serial.print(numCards);
    Serial.println(" card(s) record(s) on EEPROM");
    uint8_t numFingers = getNumFingers();
    Serial.print("I have ");     // stores the number of ID's in EEPROM
    Serial.print(numFingers);
    Serial.println(" finger(s) record(s) on EEPROM");
    masterMode();
    showWaitingAccessMessage();
  } 
  else {
    String currentUserName;
    cardVerified = cardExists(readCard);
    if (cardVerified) { // If not, see if the card is in the EEPROM
      Serial.println("Welcome, You shall pass with RFID");
      showAllowedMessage();
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, LOW);
      logAccess(AccessType::RFID, findCardIndex(readCard));
      
      if (getUserNameByAccess(AccessType::RFID, findCardIndex(readCard), currentUserName)) {
        showWelcomeMessage(currentUserName);
      } else {
        Serial.println("Error retrieving user name on RFID Access");
        showUserNotFoundError();
        showWaitingAccessMessage();          
      }

    }else if(fingerExists == 1){
      Serial.println("Welcome, You shall pass with finger");
      showAllowedMessage();
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, LOW);
      logAccess(AccessType::FINGER, fingerIdRead);

      if (getUserNameByAccess(AccessType::FINGER, fingerIdRead, currentUserName)) {
        showWelcomeMessage(currentUserName);
      } else {
        Serial.println("Error retrieving user name on Finger Access");
        showUserNotFoundError();
        showWaitingAccessMessage();
      }
    }

    else if(fingerExists == 2 || !cardVerified) {      // If not, show that the ID was not valid
      Serial.println("You shall not pass");
      showDaniedMessage();
      soundDenied();
    }
  }
  
}
