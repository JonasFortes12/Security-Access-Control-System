#include <Adafruit_Fingerprint.h>
#include "firebaseAccess.h"
#include "lcd.h" 


// Variables for storing user data coming from Firebase
String pendingUsernameForUserFinger;
String pendingEmailForUserFinger;

// Default password of DY50 module
const uint32_t password = 0x0;

Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial2, password);

uint16_t getFirstFreeSlot(void) {
    for (uint16_t i = 1; i <= fingerprintSensor.capacity; i++) {
      uint8_t result = fingerprintSensor.loadModel(i);
      if (result == FINGERPRINT_OK) {
        continue; // Slot is not free, move to the next slot
      } else if (result == FINGERPRINT_BADLOCATION) {
        // Slot is out of range, no more free slots available
        return 0;
      } else if (result == FINGERPRINT_PACKETRECIEVEERR) {
        // Communication error occurred, return 0
        return 0;
      }
      // Slot is free
      return i;
    }
    return 0; // No free slot found
  }

void initFingerSensor(){
    // Inicializa o sensor
    fingerprintSensor.begin(57600);

    // Verifica se a senha está correta
    if (!fingerprintSensor.verifyPassword()){
        // Se chegou aqui significa que a senha está errada ou o sensor está problemas de conexão
        Serial.println(F("Não foi possível conectar ao sensor. Verifique a senha ou a conexão"));
    }
}

uint8_t getNumFingers(){

    fingerprintSensor.getTemplateCount();

    return fingerprintSensor.templateCount;
}

void deleteFinger(uint8_t position){

    //Apaga a digital nesta posição
    if(fingerprintSensor.deleteModel(position) != FINGERPRINT_OK){
        Serial.println(F("Erro ao apagar digital"));
    }
    else{
        Serial.println(F("Digital apagada com sucesso!!!"));
    }

}

void storeFinger(uint8_t option){
    String currentUserName;

    uint16_t position = getFirstFreeSlot();// used as ID of the finger
    showPutFingerMessage();

    // Espera até pegar uma imagem válida da digital
    while (fingerprintSensor.getImage() != FINGERPRINT_OK);

    // Converte a imagem para o primeiro padrão
    if (fingerprintSensor.image2Tz(1) != FINGERPRINT_OK){
        // Se chegou aqui deu erro, então abortamos os próximos passos
        Serial.println(F("Erro image2Tz 1"));
        return;
    }

    //Verificar se o dedo existe
    //Procura por este padrão no banco de digitais
    // uint8_t storaged_position = fingerprintSensor.fingerFastSearch();
    if (option == 1){ // se existe, remove
        //Se chegou aqui significa que a digital foi encontrada
        Serial.println("Digital encontrada! Apagando...");
        showTakeOffFingerMessage();

        // Mostramos a posição onde a digital estava salva e a confiança
        uint8_t fingerId = fingerprintSensor.fingerID;

        if (getUserNameByAccess(AccessType::FINGER, fingerId, currentUserName)) {
          showRemovingUser(currentUserName);
        }

        deleteFinger(fingerId); 
        // Apaga o registro no Firebase
        if (deleteUserFinger(fingerId)) {
            Serial.println("✅ Usuário Finger removido com sucesso do Firebase.");
            showRemoveConfirmated(currentUserName);
        } else {
            Serial.println("❌ Falha ao remover usuário Finger do Firebase.");
            showUserNotFoundError();
            showEntryMasterModeMessage();
        }
        soundCardRemoved();
        delay(800);
        return;
    } else { // se não existe, add
        
        Serial.println("Digital não encontrada! Cadastrando...");
        Serial.println(F("Tire o dedo do sensor"));
        showTakeOffFingerMessage();


        // Espera até tirar o dedo
        while (fingerprintSensor.getImage() != FINGERPRINT_NOFINGER);

        // Antes de guardar precisamos de outra imagem da mesma digital
        Serial.println(F("Encoste o mesmo dedo no sensor"));
        showPutFingerMessage();

        // Espera até pegar uma imagem válida da digital
        while (fingerprintSensor.getImage() != FINGERPRINT_OK);

        // Converte a imagem para o segundo padrão
        if (fingerprintSensor.image2Tz(2) != FINGERPRINT_OK){
            // Se chegou aqui deu erro, então abortamos os próximos passos
            Serial.println(F("Erro image2Tz 2"));
            return;
        }

        // Cria um modelo da digital a partir dos dois padrões
        if (fingerprintSensor.createModel() != FINGERPRINT_OK){
            // Se chegou aqui deu erro, então abortamos os próximos passos
            Serial.println(F("Erro createModel"));
            return;
        }

        // Guarda o modelo da digital no sensor
        if (fingerprintSensor.storeModel(position) != FINGERPRINT_OK){
            // Se chegou aqui deu erro, então abortamos os próximos passos
            Serial.println(F("Erro storeModel"));
            return;
        }

        // Registrar no Firebase, se houver usuário pendente
        if (getPendingUser(pendingUsernameForUserFinger, pendingEmailForUserFinger)) {
            showRegisteringUser(pendingUsernameForUserFinger);

          if (!registerUserFinger(pendingUsernameForUserFinger, pendingEmailForUserFinger, position)) {
            Serial.println("Falha ao registrar usuário com FingerID no Firebase.");
          } else {
            Serial.println("Usuário registrado com FingerID no Firebase.");
            showRegisterConfirmated(pendingUsernameForUserFinger);
          }
          // Limpa dados locais
          pendingUsernameForUserFinger = "";
          pendingEmailForUserFinger = "";
          clearPendingUser();
        } else {
          Serial.println("Nenhum usuário pendente encontrado.");
        }


        // Se chegou aqui significa que todos os passos foram bem sucedidos
        Serial.println(F("Sucesso!!!"));
        soundCardDefined();
        delay(800);
    }
    
}

bool searchFinger(){
    Serial.println("Procurando Digital");
    //Espera até pegar uma imagem válida da digital
    while (fingerprintSensor.getImage() != FINGERPRINT_OK);
    // if(fingerprintSensor.getImage() != FINGERPRINT_OK){
    //     return false;
    // }

    //Converte a imagem para o padrão que será utilizado para verificar com o banco de digitais
    if (fingerprintSensor.image2Tz() != FINGERPRINT_OK){
        //Se chegou aqui deu erro, então abortamos os próximos passos
        Serial.println(F("Erro image2Tz"));
        return false;
    }
    
    //Procura por este padrão no banco de digitais
    if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK){
        //Se chegou aqui significa que a digital não foi encontrada
        Serial.println(F("Digital não encontrada"));
        return false;
    }

    //Se chegou aqui a digital foi encontrada
    return true;
    //Mostramos a posição onde a digital estava salva e a confiança
    //Quanto mais alta a confiança melhor
    Serial.print(F("Digital encontrada com confiança de "));
    Serial.print(fingerprintSensor.confidence);
    Serial.print(F(" na posição "));
    Serial.println(fingerprintSensor.fingerID);

}

// Returns 0 when no finger is detected | 1 when finger detected and exists | 2 when finger detected but not exists
uint8_t readFinger(uint8_t *fingerIdRead) {
    
    *fingerIdRead = -1; // valor padrão: nenhum ID
    
    if(fingerprintSensor.getImage() != FINGERPRINT_OK){
        return 0;
    }

    //Converte a imagem para o padrão que será utilizado para verificar com o banco de digitais
    if (fingerprintSensor.image2Tz() != FINGERPRINT_OK){
        //Se chegou aqui deu erro, então abortamos os próximos passos
        Serial.println(F("Erro image2Tz"));
        return 0;
    }
    
    //Procura por este padrão no banco de digitais
    if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK){
        //Se chegou aqui significa que a digital não foi encontrada
        Serial.println(F("Digital não encontrada"));
        return 2;
    }

    //Se chegou aqui significa que a digital foi encontrada
    *fingerIdRead = fingerprintSensor.fingerID; // Armazena o ID da digital lida


    //Se chegou aqui a digital foi encontrada
    return 1;



}

