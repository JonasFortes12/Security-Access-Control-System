#include "lcd.cpp"

void setupLCD();

void clearLCD();

void showMessage(char msg[], uint8_t line);

void showAllowedMessage();

void showDaniedMessage();

void showEntryMasterModeMessage();

void showRegisteringUser(const String &userName);

void showRegisterConfirmated(const String &userName);

void showWelcomeMessage(const String &userName);

void showRemovingUser(const String &userName);

void showRemoveConfirmated(const String &userName);
