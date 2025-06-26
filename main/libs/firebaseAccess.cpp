#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "firebaseAccess.h"
#include <WiFi.h>
#include <vector>
#include <time.h>  // Para timestamp com NTP, se configurado

#define WIFI_SSID "AP01_VALLEY"
#define WIFI_PASSWORD "amontada"
#define API_KEY "AIzaSyCYQTzRRibzV0ASConlxHp-lpvmKhnNGOs"
#define DATABASE_URL "https://security-access-control-b3a72-default-rtdb.firebaseio.com"

FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

void setupFirebase() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(200);

  Serial.println("WiFi conectado: " + WiFi.localIP().toString());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = ""; // uso anônimo

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase configurado com sucesso!");
}

// Função auxiliar para gerar ID
static String genId() {
  return String("user_") + String(millis());
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
  js.set("createdAt", millis());  // timestamp alternativo

  String id = genId();
  bool ok = Firebase.RTDB.setJSON(&fbdo, "/users/" + id, &js);
  if (ok) Firebase.RTDB.deleteNode(&fbdo, "/pendingUser/current");
  return ok;
}

// 3) Cadastra usuário por RFID
bool registerUserRFID(const String &username, const String &email, const String &cardId) {
  FirebaseJson js;
  js.set("username", username);
  js.set("email", email);
  js.set("rfid", cardId);
  js.set("createdAt", millis());  // timestamp alternativo

  String id = genId();
  bool ok = Firebase.RTDB.setJSON(&fbdo, "/users/" + id, &js);
  if (ok) Firebase.RTDB.deleteNode(&fbdo, "/pendingUser/current");
  return ok;
}

// Auxiliar: encontra caminho do usuário
static bool findUserBy(const String &node, const String &field, const String &value, String &outPath) {
  if (!Firebase.RTDB.getJSON(&fbdo, node)) return false;

  FirebaseJson &js = fbdo.to<FirebaseJson>();
  size_t len = js.iteratorBegin();

  for (size_t i = 0; i < len; i++) {
    FirebaseJson::IteratorValue it = js.valueAt(i);
    String key = it.key;
    String path = node + "/" + key + "/" + field;

    if (Firebase.RTDB.getString(&fbdo, path)) {
      if (fbdo.stringData() == value) {
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
  if (findUserBy("/users", "fingerId", String(fingerId), path))
    return Firebase.RTDB.deleteNode(&fbdo, path);
  return false;
}

// 5) Remove usuário por RFID
bool deleteUserRFID(const String &cardId) {
  String path;
  if (findUserBy("/users", "rfid", cardId, path))
    return Firebase.RTDB.deleteNode(&fbdo, path);
  return false;
}

// 6) Loga tentativa de acesso
bool logAccess(const String &type, const String &key) {
  String userPath;
  if (!findUserBy("/users", type, key, userPath)) return false;

  FirebaseJson log;
  log.set("userPath", userPath);
  log.set("accessType", type);
  log.set("accessKey", key);
  log.set("timestamp", millis());  // ou use time(NULL) se NTP configurado

  String id = String("log_") + String(millis());
  return Firebase.RTDB.setJSON(&fbdo, "/logs/" + id, &log);
}
