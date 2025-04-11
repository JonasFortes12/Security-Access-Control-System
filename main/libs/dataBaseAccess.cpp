#include <WiFi.h>                          //importa biblioteca para conectar esp32 com wifi
#include <IOXhop_FirebaseESP32.h>          //importa biblioteca para esp32 se comunicar com firebase
#include <ArduinoJson.h>    



#define WIFI_SSID "...."                 
#define WIFI_PASSWORD "...."             
#define FIREBASE_HOST "...."   
#define FIREBASE_AUTH "....." 


void setupFirebaseConnection(){
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");       //imprime "Conectando ao wifi"
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void regiterAccess(String date, String time){
    // String path = "/access/" + date + "/" + time;
    // Firebase.setBool(path, true);
    // Serial.println("Access registered at: " + path);
}

void clearDataBase(){
    // Firebase.clear("/access/");
    // Serial.println("Database cleared.");
}