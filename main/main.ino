#include "libs/services.h"
#include "libs/firebaseAccess.h"

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

  setupLCD();

  //Init Serial 
  Serial.begin(9600);  // Initialize serial communications with PC   

  checkMasterDefinition();

  //init firebase connection
  setupFirebase();

  String pendingUsername;
  String pendingEmail;

  if (getPendingUser(pendingUsername, pendingEmail)) {
    Serial.println("Usuário pendente encontrado:");
    Serial.println("Nome: " + pendingUsername);
    Serial.println("Email: " + pendingEmail);
  } else {
    Serial.println("Nenhum usuário pendente encontrado.");
  }

  //TODO: Alterar a definição de createAt ao registrar usuário: Usar formato de data e hora
  // Register user by finger (testing)
  if (registerUserFinger(pendingUsername, pendingEmail, 1)) {
    Serial.println("Usuário registrado com sucesso por digital.");
  } else {
    Serial.println("Falha ao registrar usuário por digital.");
  }

  delay(3000); // dá tempo do Firebase "indexar" a escrita

  //TODO: Alterar a definição de timestamp ao registrar log: Usar formato de data e hora
  //TODO: Alterar a definição de log id ao registrar log: Usar formato de data e hora
  //TODO: Verificar falha ao registrar log, mesmo com o usuário já resgistrado(fingerId = 1)
  // Log access (testing)
  if (logAccess("fingerId", "1")) {
    Serial.println("Acesso registrado com sucesso.");
  } else {
    Serial.println("Falha ao registrar acesso.");
  }
  
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {

  do { // Trying to scan a card
    successRead = tryScanAccessMethod(&fingerExists);
  } while (!successRead);   //the program will not go further while you are not getting a successful read
  


  if ( isMaster(readCard)) {    // If scanned card's ID matches Master Card's ID - enter program mode
    soundEntryMasterMode();
    showEntryMasterModeMessage();
    Serial.println("Hello Master - Entered Program Mode");
    uint8_t numCards = getNumCards();   // Read the first Byte of EEPROM that
    Serial.print("I have ");     // stores the number of ID's in EEPROM
    Serial.print(numCards);
    Serial.println(" card(s) record(s) on EEPROM");
    uint8_t numFingers = getNumFingers();
    Serial.print("I have ");     // stores the number of ID's in EEPROM
    Serial.print(numFingers);
    Serial.println(" finger(s) record(s) on EEPROM");
    masterMode();
  } 
  else {
    cardVerified = cardExists(readCard);
    if (cardVerified) { // If not, see if the card is in the EEPROM
      Serial.println("Welcome, You shall pass");
      showAllowedMessage();
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, HIGH);

    }else if(fingerExists == 1){
      Serial.println("Welcome, You shall pass with finger");
      showAllowedMessage();
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, HIGH);
    }

    else if(fingerExists == 2 || !cardVerified) {      // If not, show that the ID was not valid
      Serial.println("You shall not pass");
      showDaniedMessage();
      soundDenied();
    }
  }
  
}
