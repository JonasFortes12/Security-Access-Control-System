#include "libs/services.h"

uint8_t fingerExists = 0;
bool cardVerified = false;

///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {

   // pins definitions
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW);
  pinMode(WIPEBUTTON_PIN, INPUT);

  //Init RFID
  initRFIDModule();

  //Init DY50
  initFingerSensor();

  //Init Serial 
  Serial.begin(9600);  // Initialize serial communications with PC   

  checkMasterDefinition();
  
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {

  do { // Trying to scan a card
    successRead = tryScanAccessMethod(&fingerExists);
  } while (!successRead);   //the program will not go further while you are not getting a successful read
  


  if ( isMaster(readCard)) {    // If scanned card's ID matches Master Card's ID - enter program mode
    soundEntryMasterMode();
    Serial.println("Hello Master - Entered Program Mode");
    uint8_t numCards = getNumCards();   // Read the first Byte of EEPROM that
    Serial.print("I have ");     // stores the number of ID's in EEPROM
    Serial.print(numCards);
    Serial.println("card(s) record(s) on EEPROM");
    uint8_t numFingers = getNumFingers();
    Serial.print("I have ");     // stores the number of ID's in EEPROM
    Serial.print(numFingers);
    Serial.println("finger(s) record(s) on EEPROM");
    masterMode();
  } 
  else {
    cardVerified = cardExists(readCard);
    if (cardVerified) { // If not, see if the card is in the EEPROM
      Serial.println("Welcome, You shall pass");
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, HIGH);

    }else if(fingerExists == 1){
      Serial.println("Welcome, You shall pass with finger");
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, HIGH);
    }

    else if(fingerExists == 2 || !cardVerified) {      // If not, show that the ID was not valid
      Serial.println("You shall not pass");
      soundDenied();
    }
  }
  
}
