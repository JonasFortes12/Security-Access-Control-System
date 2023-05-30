#include "pins.h"
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>    // Library for Mifare RC522 Devices

MFRC522 mfrc522(SDA, RST);  // Create MFRC522 instance


void setMaster(byte card[]){
  for ( uint8_t j = 0; j < 4; j++ ) {        // Loop 4 times
    EEPROM.write( initialGapMemory + j, card[j] );  // Write scanned PICC's UID to EEPROM, start from address 3
    EEPROM.commit();      
  }
}

//////////////////////////////////////// Read an ID (RFID token) from EEPROM //////////////////////////////
void readCardByIndex(uint8_t index, byte* storedCard) {
  uint8_t start = (index * 4) + initialGapMemory;  // Figure out starting position
  for (uint8_t i = 0; i < 4; i++) {  // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);  // Assign values read from EEPROM to array
  }
}

//////////////////////////////////////// Read MasterCard from EEPROM //////////////////////////////
void getMaster(byte* storedCard) {
  readCardByIndex(0, storedCard);
}

///////////////////////////////////////// Compare if two cards are equals ///////////////////////////////////
bool compare(byte cardA[], byte cardB[]) {
  for (uint8_t k = 0; k < 4; k++) {  // Loop 4 times
    if (cardA[k] != cardB[k])  // If a != b then set match = false, one fails, all fail
      return false;
  }
  return true;
}

///////////////////////////////////////// Check if card is Master   ///////////////////////////////////
bool isMaster(byte card[]){
  byte master[4];
  getMaster(master);

  bool match = compare(master, card);
  
  return match;
}

///////////////////////////////////////// Check if MasterCard exists in EEPROM ///////////////////////////////////
bool masterExists() {
  uint8_t value;
  uint8_t countZero = 0; 
  for (uint8_t i = 1; i <= 4; i++) {  // Loop once for each EEPROM entry
    value = EEPROM.read(i);
    if (value == 0){
      countZero++;
    }
  }
  if (countZero >= 4){
    return false;
  }else{
    return true;
  }
}


///////////////////////////////////////// get number of cards in EEPROM ///////////////////////////////////
uint8_t getNumCards(){
  uint8_t nCards = EEPROM.read(slotMemory_nCards); // Get the number of used spaces, position 0 stores the number of ID cards
  return nCards; 
}

///////////////////////////////////////// increment and decrement number of cards in EEPROM ///////////////////////////////////
void incrementNumCards(){
  uint8_t num = getNumCards();  // Get the number of used spaces, position 0 stores the number of ID cards
  num++;  // Increment the counter by one
  EEPROM.write(slotMemory_nCards, num);  // Write the new count to the counter
  EEPROM.commit();
}
void decrementNumCards(){
  uint8_t num = getNumCards();  // Get the number of used spaces, position 0 stores the number of ID cards
  num--;  // Decrement the counter by one
  EEPROM.write(slotMemory_nCards, num);  // Write the new count to the counter
  EEPROM.commit();
}


///////////////////////////////////////// Check if cards exists in EEPROM ///////////////////////////////////
bool cardExists(byte card[]) {
  uint8_t count = getNumCards();  // Read the first Byte of EEPROM that stores the number of cards
  byte storedCard[4];
  for (uint8_t i = 1; i < count; i++) {  // Loop once for each EEPROM entry
    readCardByIndex(i, storedCard);  // Read an ID from EEPROM and store it in storedCard
    if (compare(card, storedCard)) {  // Check to see if the storedCard read from EEPROM matches
      return true;
    }
  }
  return false;
}

///////////////////////////////////////// Write card in an espeficied index ///////////////////////////////////
void writeCardInIndex(byte cardID[], uint8_t index){  // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t start = (index * 4) + initialGapMemory;
    for (uint8_t j = 0; j < 4; j++) {  // Loop 4 times
      EEPROM.write(start + j, cardID[j]);  // Write the array values to EEPROM in the right position
    }
    EEPROM.commit();
}

///////////////////////////////////////// Add ID to EEPROM ///////////////////////////////////
bool writeNewCard(byte cardID[]) {
  if (!cardExists(cardID)) {  // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t newIndex = getNumCards() + 1;
    writeCardInIndex(cardID, newIndex);
    incrementNumCards();
    return true;
  } else {
    return false;
  }
}

///////////////////////////////////////// Find card index in EEPROM   ///////////////////////////////////
uint8_t findCardIndex( byte card[] ) {
  uint8_t count = getNumCards();       // Read the first Byte of EEPROM that
  byte storedCard[4];
  for ( uint8_t i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readCardByIndex(i, storedCard);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( compare( card, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
    }
  }
  return -1;
}

///////////////////////////////////////// Remove card from EEPROM   ///////////////////////////////////
void deleteCard( byte card[] ) {
  if ( cardExists(card) ) {     // Before we delete from the EEPROM, check to see if we have this card!
    
    uint8_t num = getNumCards();   // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t i;
    uint8_t index = findCardIndex(card);   // Figure out the slot number of the card to delete
    byte storedCard[4];
    for ( i = index; i <= num; i++ ) {         // Loop the card shift times
      readCardByIndex(i+1, storedCard);
      writeCardInIndex(storedCard, i);
    }
    decrementNumCards();
  }
}


///////////////////////////////////////// Read RFID UID ///////////////////////////////////
bool readRFID(byte card[]) {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return false;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  for ( uint8_t i = 0; i < 4; i++) {  //
    card[i] = mfrc522.uid.uidByte[i];
  }
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}
