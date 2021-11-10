#ifndef SERIAL_CONTROLLER
#define SERIAL_CONTROLLER

#include "protocol.h"
#include <stdio.h>

char control;

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_RCV, STOP, ERROR} SUFrameState;
//typedef enum {...} IFrameState;

SUFrameState SUFrameStateMachine(SUFrameState currentState, char byte);
//IFrameState IFrameStateMachine(IFrameState currentState, char byte);

int receiveFrame(int fd, int timer, char *frame);

#endif // SERIAL_CONTROLLER