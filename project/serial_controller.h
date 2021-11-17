#ifndef SERIAL_CONTROLLER
#define SERIAL_CONTROLLER

#include "protocol.h"
#include "serial_config.h"
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_RCV, STOP, ERROR} SUFrameState;
//typedef enum {...} IFrameState;

int openSerial(char *path, struct termios *oldConfig);
int closeSerial(int fd, struct termios oldConfig);

SUFrameState SUFrameStateMachine(SUFrameState currentState, char byte);
//IFrameState IFrameStateMachine(IFrameState currentState, char byte);

int receiveFrame(int fd, int timer, char *frame);

#endif // SERIAL_CONTROLLER