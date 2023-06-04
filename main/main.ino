#include "libs/services.h"



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

  tryScanAccessMethod();


  if ( isMaster(readCard)) {    // If scanned card's ID matches Master Card's ID - enter program mode
    soundEntryMasterMode();
    Serial.println("Hello Master - Entered Program Mode");
    uint8_t numCards = getNumCards();   // Read the first Byte of EEPROM that
    Serial.println("I have ");     // stores the number of ID's in EEPROM
    Serial.println(numCards);
    Serial.println(" record(s) on EEPROM");
    masterMode();
  } 
  else {
    if ( cardExists(readCard)) { // If not, see if the card is in the EEPROM
      Serial.println("Welcome, You shall pass");
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, HIGH);
      successRead = false;
    }else if(searchFinger()){
      Serial.println("Welcome, You shall pass with finger");
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, HIGH);
      successRead = false;
    }
    else {      // If not, show that the ID was not valid
      Serial.println("You shall not pass");
      soundDenied();
      successRead = false;

    }
  }
  
}
