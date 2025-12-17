#include "RFID.h"
#include "sounds.h"
#include "DY50.h"
#include "pins.h"
#include "firebaseAccess.h"

// bool masterMode = false;  // initialize programming mode to false
bool successRead = false;    // Variable integer to keep if we have Successful Read from Reader

byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM

uint8_t fingerExists = 0; // Variable to keep if we have a finger detected or not
uint8_t fingerIdRead = -1; // Variable to keep the ID of the finger read, default is -1 (no ID)
bool cardVerified = false; // Variable to keep if the card is verified or not


// Variables for storing user data coming from Firebase
String pendingUsername;
String pendingEmail;

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
  fingerprintSensor.emptyDatabase();
  soundAllCardsRemoved();
  Serial.println("Cleaning All Cards...");
}

void checkMasterDefinition(){
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

bool tryScanAccessMethod(uint8_t* fingerExists, uint8_t* fingerIdRead) {
  executeAfterTimeInState(deleteAllCards, 10, WIPEBUTTON_PIN, HIGH);
  memset(readCard, 0, sizeof(readCard));
  // sets successRead to 1 when we get read from reader otherwise 0
  // sets successRead to 1 when we get a finger
  *fingerExists = readFinger(fingerIdRead); // Read a finger from DY50 Module and check if it exists or not
  if(readRFID(readCard) || *fingerExists != 0){
    Serial.println("LEU entrada de cartão ou dedo");
    return true;
  }else{
    Serial.println("Não LEU entrada de cartão ou dedo");
    return false;
  }
}

void masterMode(){
  String currentUserName;

  do{
    memset(readCard, 0, sizeof(readCard)); // Limpa o readCard para ler um novo cartão
    
    readRFID(readCard); // Read a card from RFID Module

    // Read a finger from DY50 Module e verifica se já está cadastrado ou não
    // retorna 0 quando não há dedo, 1 quando o dedo existe e 2 quando o dedo não existe
    uint8_t fingerAnswer = readFinger(&fingerIdRead); 
    
    if ( isMaster(readCard) ) { //When in program mode check First If master card scanned again to exit program mode
      Serial.println("Master Card scanned");
      soundExitMasterMode();
      Serial.println("Exiting Program Mode");
      break;
    } 
    else {
      
      if ( cardExists(readCard)) { // If scanned card is known delete it
        Serial.println("I know this card, removing...");
        if (getUserNameByAccess(AccessType::RFID, findCardIndex(readCard), currentUserName)) {
          showRemovingUser(currentUserName);
        }
        

        if(deleteUserRFID(findCardIndex(readCard))){
          Serial.println("Usuário Card removido com sucesso do Firebase.");
          showRemoveConfirmated(currentUserName);
        } else {
          Serial.println("Falha ao remover usuário card do Firebase.");
          //TODO: Mensagem: Erro ao remover usuario.
        }
        
        deleteCard(readCard);
        soundCardRemoved();
      }
      
      else if(fingerAnswer != 0){ // If some finger is detected
        storeFinger(fingerAnswer);
      }
      
      else if(isCardNull(readCard)){ // do nothing
        continue;
      }
      
      else{ // If scanned card is NOT known add it
        Serial.println("I do not know this card, adding...");
        writeNewCard(readCard);

        // Check if there is a pending user in Firebase and register the card in Firebase
        if (getPendingUser(pendingUsername, pendingEmail)) {
           Serial.println("Usuário pego Username: " + pendingUsername);
           Serial.println("Usuário pego Email: " + pendingEmail);
           
          showRegisteringUser(pendingUsername);
          uint8_t readCardId =  findCardIndex(readCard);// Get the index of the card in EEPROM
          if (!registerUserRFID(pendingUsername, pendingEmail, readCardId)) {
             Serial.println("Falha ao registrar usuário com CardId no Firebase.");
          }
          showRegisterConfirmated(pendingUsername);

          // Clear the pending user data
          pendingUsername = "";
          pendingEmail = "";
          clearPendingUser();

        } else {
          //TODO: Mensagem: "Cadastro não iniciado!"
          Serial.println("Nenhum usuário pendente encontrado.");
        }

        soundCardDefined();
      }
    }
  }while (true);
  
  clearLCD();

}





















