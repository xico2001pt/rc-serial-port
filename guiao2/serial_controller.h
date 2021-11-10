#ifndef SERIAL_CONTROLLER
#define SERIAL_CONTROLLER

#include "protocol.h"
#include <stdio.h>
#include <signal.h>

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_RCV, STOP, ERROR} SUFrameState;
//typedef enum {...} IFrameState;

SUFrameState SUFrameStateMachine(SUFrameState currentState, char byte);
//IFrameState IFrameStateMachine(IFrameState currentState, char byte);

int communicateFrame(int fd, int attempts, int timer, char *data, char* status, int writeFirst);  // TODO: fd, attempts e timer podiam ser uma struct, como nos slides
int receiveFrame(int fd, int timer, char *frame);

#endif // SERIAL_CONTROLLER