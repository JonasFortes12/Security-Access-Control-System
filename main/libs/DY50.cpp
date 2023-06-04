#include <Adafruit_Fingerprint.h>

// Default password of DY50 module
const uint32_t password = 0x0;

Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&Serial2, password);

void initFingerSensor(){
    // Inicializa o sensor
    fingerprintSensor.begin(57600);

    // Verifica se a senha está correta
    if (!fingerprintSensor.verifyPassword()){
        // Se chegou aqui significa que a senha está errada ou o sensor está problemas de conexão
        Serial.println(F("Não foi possível conectar ao sensor. Verifique a senha ou a conexão"));
    }
}

void storeFinger(int position){
    Serial.println(F("Encoste o dedo no sensor para armazenar"));

    // Espera até pegar uma imagem válida da digital
    while (fingerprintSensor.getImage() != FINGERPRINT_OK);

    // Converte a imagem para o primeiro padrão
    if (fingerprintSensor.image2Tz(1) != FINGERPRINT_OK){
        // Se chegou aqui deu erro, então abortamos os próximos passos
        Serial.println(F("Erro image2Tz 1"));
        return;
    }

    Serial.println(F("Tire o dedo do sensor"));

    delay(2000);

    // Espera até tirar o dedo
    while (fingerprintSensor.getImage() != FINGERPRINT_NOFINGER);

    // Antes de guardar precisamos de outra imagem da mesma digital
    Serial.println(F("Encoste o mesmo dedo no sensor"));

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

    // Se chegou aqui significa que todos os passos foram bem sucedidos
    Serial.println(F("Sucesso!!!"));
}

void searchFinger(){
    Serial.println(F("Encoste o dedo no sensor para buscar"));

    //Espera até pegar uma imagem válida da digital
    while (fingerprintSensor.getImage() != FINGERPRINT_OK);

    //Converte a imagem para o padrão que será utilizado para verificar com o banco de digitais
    if (fingerprintSensor.image2Tz() != FINGERPRINT_OK){
        //Se chegou aqui deu erro, então abortamos os próximos passos
        Serial.println(F("Erro image2Tz"));
        return;
    }
    
    //Procura por este padrão no banco de digitais
    if (fingerprintSensor.fingerFastSearch() != FINGERPRINT_OK){
        //Se chegou aqui significa que a digital não foi encontrada
        Serial.println(F("Digital não encontrada"));
        return;
    }

    //Se chegou aqui a digital foi encontrada
    //Mostramos a posição onde a digital estava salva e a confiança
    //Quanto mais alta a confiança melhor
    Serial.print(F("Digital encontrada com confiança de "));
    Serial.print(fingerprintSensor.confidence);
    Serial.print(F(" na posição "));
    Serial.println(fingerprintSensor.fingerID);

}
