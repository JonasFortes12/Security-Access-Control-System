#include "services.cpp"

void checkMasterDefinition();

void setPinInStateForTime(int timeInSeconds, int pin, int state);

void executeAfterTimeInState(void (*functionToExecute)(), unsigned long time, int pin, int state);

void deleteMasterCard();

void deleteAllCards();

void tryScanAccessMethod();

void masterMode();