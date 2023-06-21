#include "services.cpp"

void setupLCD();

void showMessage(char msg[], uint8_t line);

void checkMasterDefinition();

void setPinInStateForTime(int timeInSeconds, int pin, int state);

void executeAfterTimeInState(void (*functionToExecute)(), unsigned long time, int pin, int state);

void deleteMasterCard();

void deleteAllCards();

bool tryScanAccessMethod(uint8_t* fingerExists);

void masterMode();