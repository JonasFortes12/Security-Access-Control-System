#include "libs/services.h"
#include "libs/firebaseAccess.h"



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

  //Init Display
  setupLCD();

  //Init Serial 
  Serial.begin(9600);  // Initialize serial communications with PC   

  // Check if Master Card is defined
  checkMasterDefinition();

  //init firebase connection
  setupFirebase();

  //init time for logging timestamp
  setupTime();

   // === TESTE: fluxo de registro e log ===
  Serial.println("🚧 Iniciando teste de fluxo Firebase...");

  // 1. Pegar usuário pendente
  if (getPendingUser(pendingUsername, pendingEmail)) {
    Serial.println("👤 Usuário pendente encontrado:");
    Serial.println("Nome: " + pendingUsername);
    Serial.println("Email: " + pendingEmail);

    // 2. Cadastrar com um ID fictício (simulando RFID = 33 e fingerId = 44)
    const uint8_t testCardId = 1;
    const uint8_t testFingerId = 1;

    bool registeredCard = registerUserRFID(pendingUsername, pendingEmail, testCardId);
    bool registeredFinger = registerUserFinger(pendingUsername, pendingEmail, testFingerId);

    if (registeredCard && registeredFinger) {
      Serial.println("✅ Usuário registrado com sucesso com RFID e Finger!");

      // 3. Fazer log de acesso para ambos
      bool loggedCard = logAccess(AccessType::RFID, testCardId);
      bool loggedFinger = logAccess(AccessType::FINGER, testFingerId);

      if (loggedCard && loggedFinger) {
        Serial.println("📝 Log de acesso realizado com sucesso!");
      } else {
        Serial.println("⚠️ Falha ao registrar log de acesso.");
      }
    } else {
      Serial.println("❌ Falha no registro do usuário.");
    }
  } else {
    Serial.println("ℹ️ Nenhum usuário pendente encontrado.");
  }

  Serial.println("✅ Teste de fluxo concluído.");
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {

  do { // Trying to scan a card
    successRead = tryScanAccessMethod(&fingerExists, &fingerIdRead);  // sets successRead to 1 when we get read from reader otherwise 0
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
      logAccess(AccessType::RFID, findCardIndex(readCard));
      showAllowedMessage();
      soundAllowed();
      setPinInStateForTime(2, LOCK_PIN, HIGH);

    }else if(fingerExists == 1){
      Serial.println("Welcome, You shall pass with finger");
      logAccess(AccessType::FINGER, fingerIdRead);
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
