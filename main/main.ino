#include "libs/RFID.h"

bool masterMode = false;  // initialize programming mode to false
bool successRead = false;    // Variable integer to keep if we have Successful Read from Reader

byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM

bool wipeState = false;   // Wipe State (Wipe Button)
unsigned long startTime;  // Time of start of counting

#define BUZZER_PIN 33

void soundEntryMasterMode(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void soundExitMasterMode(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void soundAllowed(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void soundDenied(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

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

void myFunc(){
  Serial.println("Wipe Button pressed by 10 seconds!");
  clearMaster();
  Serial.println("Cleaning Master Card...");
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
  } else {
      Serial.println("Master Card already defined");
      executeAfterTimeInState(myFunc, 10, WIPEBUTTON_PIN, HIGH);
  }

}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {
  
  do { // Trying to scan a card
    // Verify if the Wipe Button was pressed
    if (digitalRead(WIPEBUTTON_PIN) == HIGH && !wipeState) {
      wipeState = true;             // Toggle Wipe State
      startTime = millis();         // Save time of start counting
      Serial.println("Wipe Button pressed! (In 10 seconds all cards will be erased)");
    }
    // Verify if the Wipe Button was released
    if (digitalRead(WIPEBUTTON_PIN) == LOW && wipeState) {
      wipeState = false;            // Toggle Wipe State
      unsigned long elapsedTime = millis() - startTime;  // Calculate spent time
      if (elapsedTime >= 10000) {     // Verify if spent time is equals or greater then 10 seconds
        Serial.println("Wipe Button pressed by 10 seconds!");
        clearCards();
        Serial.println("Cleaning all cards...");
      }
    }
    if (!wipeState){
      successRead = readRFID(readCard);  // sets successRead to 1 when we get read from reader otherwise 0
    }
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
      }
      else {                    // If scanned card is not known add it
        Serial.println("I do not know this card, adding...");
        writeNewCard(readCard);
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
