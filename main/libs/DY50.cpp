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

uint8_t getNumFingers(){

    fingerprintSensor.getTemplateCount();

    return fingerprintSensor.templateCount;
}


void storeFinger(){
    uint8_t position = getNumFingers() + 1;
    Serial.println(F("Encoste o dedo no sensor para armazenar"));

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
    if (fingerprintSensor.fingerFastSearch() == FINGERPRINT_OK){ // se existe, remove
        //Se chegou aqui significa que a digital foi encontrada
        // deleteFinger(x);
        return;
    } else { // se não existe, add
        Serial.println(F("Tire o dedo do sensor"));

        // delay(2000);

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
uint8_t readFinger(){
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

    //Se chegou aqui a digital foi encontrada
    return 1;



}

void deleteFinger(int position){

    //Apaga a digital nesta posição
    if(fingerprintSensor.deleteModel(position) != FINGERPRINT_OK){
        Serial.println(F("Erro ao apagar digital"));
    }
    else{
        Serial.println(F("Digital apagada com sucesso!!!"));
    }

}