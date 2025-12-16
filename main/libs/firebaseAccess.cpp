#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "firebaseAccess.h"
#include <WiFi.h>
#include <vector>
#include <time.h> 
#include "secrets.h"

FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

// Definições de tipos de acesso
enum class AccessType {
  RFID,
  FINGER
};


void setupFirebase() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado: " + WiFi.localIP().toString());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  //habilita token anônimo
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("✅ Autenticado anonimamente!");
  } else {
    Serial.printf("❌ Erro ao autenticar: %s\n", config.signer.signupError.message.c_str());
    return; // Sai para evitar continuar sem autenticação
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  delay(1000);  // Tempo para garantir que o token foi obtido

  if (Firebase.ready()) {
    Serial.println("✅ Firebase está pronto para uso.");
  } else {
    Serial.println("❌ Firebase não está pronto.");
  }

}

void setupTime() {
  // Configura o fuso horário para UTC-3 (Brasil)
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  int retry = 0;
  const int retryCount = 2;

  // Tenta sincronizar com NTP (máx. 10 tentativas)
  while (!getLocalTime(&timeinfo) && retry++ < retryCount) {
    Serial.println("⏳ Aguardando sincronização com NTP...");
    delay(600);
  }

  if (retry < retryCount) {
    Serial.println("✅ NTP sincronizado com sucesso.");
    Serial.println(&timeinfo, "🕓 Hora atual: %Y-%m-%d %H:%M:%S");
  } else {
    Serial.println("❌ Falha ao sincronizar NTP.");
  }
}

// Função auxiliar para obter o timestamp atual
String getCurrentTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01 00:00:00";  // fallback se NTP não estiver pronto
  }

  char buffer[20];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

// 1) Pega usuário pendente
bool getPendingUser(String &username, String &email) {
  if (!Firebase.RTDB.getJSON(&fbdo, "/pendingUser/current")) return false;

  FirebaseJson &js = fbdo.to<FirebaseJson>();
  FirebaseJsonData result;

  if (js.get(result, "username"))
    username = result.stringValue;
  if (js.get(result, "email"))
    email = result.stringValue;

  return true;
}

// 2) Cadastra usuário por digital
bool registerUserFinger(const String &username, const String &email, uint8_t fingerId) {
  FirebaseJson js;
  js.set("username", username);
  js.set("email", email);
  js.set("fingerId", fingerId);
  js.set("createdAt", getCurrentTimestamp()); 

  String id = "user_finger_id_" + String(fingerId);
  String path = "/users/" + id;

  Serial.println("Enviando dados para: " + path);
  Serial.println("Nome: " + username + " | Email: " + email + " | FingerID: " + String(fingerId));

  bool ok = Firebase.RTDB.setJSON(&fbdo, path, &js);

  if (ok) {
    Serial.println("✅ Usuário registrado com sucesso.");
    Firebase.RTDB.deleteNode(&fbdo, "/pendingUser/current");
  } else {
    Serial.println("❌ Falha ao registrar usuário.");
    Serial.println("Erro: " + fbdo.errorReason());
  }

  return ok;
}

// 3) Cadastra usuário por RFID
bool registerUserRFID(const String &username, const String &email, const uint8_t cardId) {
  FirebaseJson js;
  js.set("username", username);
  js.set("email", email);
  js.set("rfid", cardId);
  js.set("createdAt", getCurrentTimestamp());  // timestamp alternativo

  String id = "user_card_id_" + String(cardId);
  bool ok = Firebase.RTDB.setJSON(&fbdo, "/users/" + id, &js);
  if (ok) Firebase.RTDB.deleteNode(&fbdo, "/pendingUser/current");
  return ok;
}

// Auxiliar: encontra caminho do usuário
static bool findUserBy(const String &node, const String &field, const uint8_t &value, String &outPath) {
  if (!Firebase.RTDB.getJSON(&fbdo, node)) return false;

  FirebaseJson &js = fbdo.to<FirebaseJson>();
  size_t len = js.iteratorBegin();

  for (size_t i = 0; i < len; i++) {
    FirebaseJson::IteratorValue it = js.valueAt(i);
    String key = it.key;
    String path = node + "/" + key + "/" + field;

    if (Firebase.RTDB.getInt(&fbdo, path)) {
      if (fbdo.intData() == value) {
        outPath = node + "/" + key;
        js.iteratorEnd();
        return true;
      }
    }
  }

  js.iteratorEnd();
  return false;
}

// 4) Remove usuário por fingerId
bool deleteUserFinger(uint8_t fingerId) {
  String path;
  if (findUserBy("/users", "fingerId", fingerId, path))
    return Firebase.RTDB.deleteNode(&fbdo, path);
  return false;
}

// 5) Remove usuário por RFID
bool deleteUserRFID(const uint8_t &cardId) {
  String path;
  if (findUserBy("/users", "rfid", cardId, path))
    return Firebase.RTDB.deleteNode(&fbdo, path);
  return false;
}

// 6) Loga tentativa de acesso
bool logAccess(AccessType type, const uint8_t &key) {
  String typeAccess;
  switch (type) {
    case AccessType::RFID:
      typeAccess = "rfid";
      break;
    case AccessType::FINGER:
      typeAccess = "fingerId";
      break;
  }

  String userPath;
  if (!findUserBy("/users", typeAccess, key, userPath)) {
    Serial.println("Usuário não encontrado - Log de acesso. " + String(key));
    return false;
  }

  // Variáveis para armazenar dados do usuário
  String username;
  String email;

  // Lê o nome do usuário
  if (Firebase.RTDB.getString(&fbdo, userPath + "/username")) {
    username = fbdo.stringData();
  }

  // Lê o e-mail do usuário
  if (Firebase.RTDB.getString(&fbdo, userPath + "/email")) {
    email = fbdo.stringData();
  }

  FirebaseJson log;
  log.set("userName", username);
  log.set("userEmail", email);
  log.set("userPath", userPath);
  log.set("accessType", typeAccess);
  log.set("timestamp", getCurrentTimestamp());  // ou use time(NULL) se NTP configurado

  String id = String("log_") + millis(); // ID único baseado no tempo atual
  return Firebase.RTDB.setJSON(&fbdo, "/logs/" + id, &log);
}

// 7) Limpa o usuário pendente atual
bool clearPendingUser() {
  bool ok = Firebase.RTDB.deleteNode(&fbdo, "/pendingUser/current");
  if (ok) {
    Serial.println("Usuário pendente limpo com sucesso.");
  } else {
    Serial.println("Falha ao limpar usuário pendente.");
  }
  return ok;
}

// 7) Pega o nome do usuário
bool getUserNameByAccess(AccessType type, const uint8_t &key, String &outUserName) {
  String field;
  switch (type) {
    case AccessType::RFID:   field = "rfid";     break;
    case AccessType::FINGER: field = "fingerId"; break;
    default: return false;
  }

  String userPath;

  if (!findUserBy("/users", field, key, userPath)) {
    Serial.println("getUserNameByAccess: usuário não encontrado. key=" + String(key));
    return false;
  }

  if (!Firebase.RTDB.getString(&fbdo, userPath + "/username")) {
    Serial.println("getUserNameByAccess: falha ao ler username: " + fbdo.errorReason());
    return false;
  }

  outUserName = fbdo.stringData();
  return true;
}