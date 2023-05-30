#include "libs/RFID.h"


bool masterMode = false;  // initialize programming mode to false

bool successRead = false;    // Variable integer to keep if we have Successful Read from Reader

byte storedCard[4];   // Stores an ID read from EEPROM
byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM



///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  

  //Protocol Configuration
  size_t size = sizeof(byte) * 512; 
  EEPROM.begin(size);
  Serial.begin(9600);  // Initialize serial communications with PC   
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware

  if (!masterExists()) {
    Serial.println(F("No Master Card Defined"));
    Serial.println(F("Scan A PICC to Define as Master Card"));
    
    do {
        //Trying to read RFID
      successRead = readRFID(readCard);            // sets successRead to 1 when we get read from reader otherwise 0
    }
    while (!successRead);                  // Program will not go further while you not get a successful read
    
    setMaster(readCard);
    Serial.println(F("Master Card Defined"));
  } else {
      Serial.println(F("Master Already Card Defined"));
  }


}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {
  
  do {
    // Serial.println("Reading Card ...");
    successRead = readRFID(readCard);  // sets successRead to 1 when we get read from reader otherwise 0
  }
  while (!successRead);   //the program will not go further while you are not getting a successful read
  
  if (masterMode) {
    if ( isMaster(readCard) ) { //When in program mode check First If master card scanned again to exit program mode
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      masterMode = false;
    }
    else {
      if ( cardExists(readCard) ) { // If scanned card is known delete it
        Serial.println(F("I know this PICC, removing..."));
        deleteCard(readCard);
      }
      else {                    // If scanned card is not known add it
        Serial.println(F("I do not know this PICC, adding..."));
        writeNewCard(readCard);
      }
    }
  }else {
    if ( isMaster(readCard)) {    // If scanned card's ID matches Master Card's ID - enter program mode
      masterMode = true;
      Serial.println(F("Hello Master - Entered Program Mode"));
      uint8_t numCards = getNumCards();   // Read the first Byte of EEPROM that
      Serial.print(F("I have "));     // stores the number of ID's in EEPROM
      Serial.print(numCards);
      Serial.print(F(" record(s) on EEPROM"));
    }
    else {
      if ( cardExists(readCard) ) { // If not, see if the card is in the EEPROM
        Serial.println(F("Welcome, You shall pass"));
      }
      else {      // If not, show that the ID was not valid
        Serial.println(F("You shall not pass"));
      }
    }
  }
}
