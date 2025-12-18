#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(RS_PIN, E_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

void setupLCD(){
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void showMessage(const String &msg, uint8_t line){
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, line);
  // print the number of seconds since reset:
  lcd.print(msg);
}

void clearLCD(){
    lcd.clear();
}

void showAllowedMessage(){
    clearLCD();
    showMessage("Bem Vindo(a),",0);
}

void showWaitingAccessMessage(){
    clearLCD();
    showMessage("Aguardando", 0);
    showMessage("acesso ...", 1);
}

void showDaniedMessage(){
    clearLCD();
    showMessage("Acesso Negado!",0);
    delay(3000);
    showWaitingAccessMessage();
}

void showStartingSystemMessage(){
    clearLCD();
    showMessage("Iniciando ", 0);
    showMessage("sistema...", 1);
}

void showEntryMasterModeMessage(){
    clearLCD();
    showMessage("Modo de Cadastro", 0);
}

void showRegisteringUser(const String &userName) {
  clearLCD();
  showMessage("Registrando: ", 0);
  showMessage(userName, 1);
}

void showRegisterConfirmated(const String &userName) {
  clearLCD();
  showMessage("Registro Feito: ", 0);
  showMessage(userName, 1);
  delay(2000);
  showEntryMasterModeMessage();
}

void showWelcomeMessage(const String &userName) {
  clearLCD();
  showMessage("Bem Vindo(a),",0);
  showMessage(userName, 1);
  delay(3000);
  showWaitingAccessMessage();
}


void showRemovingUser(const String &userName) {
  clearLCD();
  showMessage("Removendo: ", 0);
  showMessage(userName, 1);
}

void showRemoveConfirmated(const String &userName) {
  clearLCD();
  showMessage("Removido: ", 0);
  showMessage(userName, 1);
  delay(2000);
  showEntryMasterModeMessage();
}

void showPutFingerMessage(){
    clearLCD();
    showMessage("Coloque o dedo", 0);
    showMessage("no sensor...", 1);
    delay(2000);
}

void showTakeOffFingerMessage(){
    clearLCD();
    showMessage("Retire o dedo", 0);
    showMessage("do sensor...", 1);
    delay(2000);
}

void showRegisterNotStartedError(){
    clearLCD();
    showMessage("Cadastro web nao", 0);
    showMessage("iniciado!", 1);
    delay(2000);
    showEntryMasterModeMessage();
}

void showUserNotFoundError(){
    clearLCD();
    showMessage("Nome usuario nao", 0);
    showMessage("encontrado!", 1);
    delay(2000);
}

