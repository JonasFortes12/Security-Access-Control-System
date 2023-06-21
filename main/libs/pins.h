/*
Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST -> GPIO 22(ESP32)
   SPI SS      SDA(SS)      10            53        D10        10               10  -> GPIO 21(ESP32)
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16  -> GPIO 23(ESP32)
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14  -> GPIO 19(ESP32)
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15  -> GPIO 18(ESP32)
*/

// RFID pins definition:
#define RST_PIN 22
#define SDA_PIN 21
#define MOSI_PIN 23
#define MISO_PIN 19
#define SCK_PIN 18


 
/* DY50 pins: 
   GPIO 16 - RX2
   GPIO 17 - TX2
*/


// Actuator pins definition:
#define WIPEBUTTON_PIN 26
#define LOCK_PIN 4
#define BUZZER_PIN 33

//Position in memory that save how many cards are recorded in flash memory 
#define slotMemory_nCards 0

// First position in EEPROM is reserved to save number of cards registered in memory
#define initialGapMemory 1


//LCD pins:
#define RS_PIN 15
#define E_PIN 2
#define D4_PIN 13
#define D5_PIN 12 
#define D6_PIN 14 
#define D7_PIN 27