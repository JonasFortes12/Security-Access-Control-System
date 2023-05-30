#include "pins.h"
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices


//////////////////////////////////////// Read an ID(RFID token) from EEPROM //////////////////////////////
int* readCardByIndex( uint8_t index ) {
  static int storedCard[4];
  uint8_t start = (index * 4 ) + lastSlotMaster;    // Figure out starting position
  for ( uint8_t i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
  return storedCard;
}

///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
bool writeCard( byte cardID[] ) {
  if ( !findID( cardID ) ) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(slotMemory_nCards);     // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write( NumberCardsRecorded_SlotMemory, num );     // Write the new count to the counter
    EEPROM.commit();
    for ( uint8_t j = 0; j < 4; j++ ) {   // Loop 4 times
      EEPROM.write( start + j, cardID[j] );  // Write the array values to EEPROM in the right position
      EEPROM.commit();
    }
    return true;
  }
  else {
    return false;
  }
}

///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
bool findID( byte card[] ) {
  uint8_t count = EEPROM.read(slotMemory_nCards);     // Read the first Byte of EEPROM that  
  for ( uint8_t i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    int* storedCard = readCardByIndex(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( compare( card, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;  // Stop looking we found it
    }
  }
  return false;
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
bool compare ( byte cardA[], byte cardB[] ) {
  for ( uint8_t k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( cardA[k] != cardB[k] )     // IF a != b then set match = false, one fails, all fail
      return false;
  }
  return true;
}


///////////////////////////////////////// Remove ID from EEPROM   ///////////////////////////////////
void deleteID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite();      // If not
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else {
    uint8_t num = EEPROM.read(NumberCardsRecorded_SlotMemory);   // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot;       // Figure out the slot number of the card
    uint8_t start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    uint8_t looping;    // The number of times the loop repeats
    uint8_t j;
    uint8_t count = EEPROM.read(NumberCardsRecorded_SlotMemory); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;      // Decrement the counter by one
    EEPROM.write( NumberCardsRecorded_SlotMemory, num );   // Write the new count to the counter
    EEPROM.commit();
    for ( j = 0; j < looping; j++ ) {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
      EEPROM.commit();
    }
    for ( uint8_t k = 0; k < 4; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);
      EEPROM.commit();
    }
    successDelete();
    Serial.println(F("Succesfully removed ID record from EEPROM"));
  }
}