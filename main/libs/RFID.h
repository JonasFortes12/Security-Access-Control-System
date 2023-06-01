#include "RFID.cpp" 

//////////////////////////////////////// Read an ID (RFID token) from EEPROM //////////////////////////////
void readCardByIndex(uint8_t index, byte* storedCard);

///////////////////////////////////////// Add ID to EEPROM ///////////////////////////////////
bool writeNewCard(byte cardID[]);

///////////////////////////////////////// get number of cards in EEPROM ///////////////////////////////////
uint8_t getNumCards();

///////////////////////////////////////// increment and decrement number of cards in EEPROM ///////////////////////////////////
void incrementNumCards();
void decrementNumCards();

//////////////////////////////////////// Write card in an espeficied index ///////////////////////////////////
void writeCardInIndex(byte cardID[], uint8_t index);

///////////////////////////////////////// Check if cards exists in EEPROM ///////////////////////////////////
bool cardExists(byte card[]);

///////////////////////////////////////// Check if MasterCard exists in EEPROM ///////////////////////////////////
bool masterExists();

/////////////////////////////////////// Read MasterCard from EEPROM //////////////////////////////
void getMaster(byte* storedCard);

///////////////////////////////////////// Compare if two cards are equals ///////////////////////////////////
bool compare(byte cardA[], byte cardB[]);

///////////////////////////////////////// Remove card from EEPROM   ///////////////////////////////////
void deleteCard( byte card[] );

///////////////////////////////////////// Find card index in EEPROM   ///////////////////////////////////
uint8_t findCardIndex( byte card[] );

///////////////////////////////////////// Check if card is Master   ///////////////////////////////////
bool isMaster(byte card[]);

///////////////////////////////////////// Read RFID UID ///////////////////////////////////
bool readRFID(byte card[]);

void setMaster(byte card[]);

void clearMaster();

void clearCards();

void setNumCards(uint8_t num);