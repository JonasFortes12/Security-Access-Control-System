#ifndef FIREBASE_ACCESS_H
#define FIREBASE_ACCESS_H
#include <Firebase_ESP_Client.h>

// Configuração externa
extern FirebaseAuth auth;
extern FirebaseConfig config;
extern FirebaseData fbdo;

#include "./firebaseAccess.cpp"


// Inicialização
void setupFirebase();

// Configuração de tempo
void setupTime();

// Funções :
bool getPendingUser(String &username, String &email);
bool registerUserFinger(const String &username, const String &email, uint8_t fingerId);
bool registerUserRFID(const String &username, const String &email, const uint8_t &cardId);
bool deleteUserFinger(uint8_t fingerId);
bool deleteUserRFID(const uint8_t &cardId);

bool logAccess(const String &type, const uint8_t &key);

#endif
