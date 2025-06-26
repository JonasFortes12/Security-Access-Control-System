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

// Funções requisitadas:
bool getPendingUser(String &username, String &email);
bool registerUserFinger(const String &username, const String &email, uint8_t fingerId);
bool registerUserRFID(const String &username, const String &email, const String &cardId);
bool deleteUserFinger(uint8_t fingerId);
bool deleteUserRFID(const String &cardId);
bool logAccess(const String &type, const String &key);

#endif
