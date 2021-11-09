#ifndef STATE_MACHINE
#define STATE_MACHINE

#include "protocol.h"

char control;

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_RCV, STOP} SUFrameState;
//typedef enum {...} IFrameState;

SUFrameState SUFrameStateMachine(SUFrameState currentState, char byte);
//IFrameState IFrameStateMachine(IFrameState currentState, char byte);

#endif // STATE_MACHINE