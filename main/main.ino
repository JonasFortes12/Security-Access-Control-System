#include "libs/RFID.h"
#include "libs/sounds.h"

bool masterMode = false;  // initialize programming mode to false
bool successRead = false;    // Variable integer to keep if we have Successful Read from Reader

byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM




void setPinInStateForTime(int timeInSeconds, int pin, int state) {
  digitalWrite(pin, state); // Define o pino como HIGH
  delay(timeInSeconds * 1000); // Converte o tempo em segundos para milissegundos
  digitalWrite(pin, !state); // Define o pino como LOW
}

void executeAfterTimeInState(void (*functionToExecute)(), unsigned long time, int pin, int state){
  unsigned long startTime = millis();
  
  while (digitalRead(pin) == state){
    if (millis() - startTime >= (time * 1000)){
      functionToExecute(); // Execute the specified function after x seconds
      break;
    }
  }
}

void deleteMasterCard(){
  Serial.println("Wipe Button pressed. In 10 seconds the master card will be erased!");
  soundCardRemoved();
  Serial.println("Cleaning Master Card...");
  clearMaster(); // After clear master the system is reloaded.
}

void deleteAllCards(){
  Serial.println("Wipe Button pressed. In 10 seconds all cards will be erased!");
  clearCards();
  soundAllCardsRemoved();
  Serial.println("Cleaning All Cards...");
}



///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW);
  pinMode(WIPEBUTTON_PIN, INPUT);
  //Protocol Configuration
  size_t size = sizeof(byte) * 512; 
  EEPROM.begin(size);
  Serial.begin(9600);  // Initialize serial communications with PC   
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware

  // Check if Master Card is not defined
  if (!masterExists()) {
    Serial.println("No Master Card defined");
    Serial.println("Scan a card to define as Master Card");
    
    do { // Trying to scan a card
      successRead = readRFID(readCard);  // sets successRead to 1 when we get read from reader otherwise 0
    }
    while (!successRead);                // Program will not go further while you not get a successful read
    
    setMaster(readCard);
    Serial.println("Master Card defined");
    soundCardDefined();
  } else {
      Serial.println("Master Card already defined");
      executeAfterTimeInState(deleteMasterCard, 10, WIPEBUTTON_PIN, HIGH);
  }

}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {
  
  do { // Trying to scan a card
   
    executeAfterTimeInState(deleteAllCards, 10, WIPEBUTTON_PIN, HIGH);

    successRead = readRFID(readCard);  // sets successRead to 1 when we get read from reader otherwise 0

  }
  while (!successRead);   //the program will not go further while you are not getting a successful read
  
  if (masterMode) { // Check if it's in Program Mode
    if ( isMaster(readCard) ) { //When in program mode check First If master card scanned again to exit program mode
      Serial.println("Master Card scanned");
      soundExitMasterMode();
      Serial.println("Exiting Program Mode");
      masterMode = false;
    } else {
      if ( cardExists(readCard) ) { // If scanned card is known delete it
        Serial.println("I know this card, removing...");
        deleteCard(readCard);
        soundCardRemoved();
      }
      else {                    // If scanned card is not known add it
        Serial.println("I do not know this card, adding...");
        writeNewCard(readCard);
        soundCardDefined();
      }
    }
  } else {
    if ( isMaster(readCard)) {    // If scanned card's ID matches Master Card's ID - enter program mode
      masterMode = true;
      soundEntryMasterMode();
      Serial.println("Hello Master - Entered Program Mode");
      uint8_t numCards = getNumCards();   // Read the first Byte of EEPROM that
      Serial.println("I have ");     // stores the number of ID's in EEPROM
      Serial.println(numCards);
      Serial.println(" record(s) on EEPROM");
    } else {
      if ( cardExists(readCard) ) { // If not, see if the card is in the EEPROM
        Serial.println("Welcome, You shall pass");
        soundAllowed();
        setPinInStateForTime(2, LOCK_PIN, HIGH);
      }
      else {      // If not, show that the ID was not valid
        Serial.println("You shall not pass");
        soundDenied();
      }
    }
  }
}
