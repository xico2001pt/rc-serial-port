#ifndef SERIAL_CONTROLLER
#define SERIAL_CONTROLLER

#include "protocol.h"
#include "serial_config.h"
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_RCV, BCC1_RCV, STOP, ERROR} FrameState;

int openSerial(char *path, struct termios *oldConfig);
int closeSerial(int fd, struct termios oldConfig);

FrameState FrameStateMachine(FrameState currentState, char byte);

int receiveFrame(int fd, int timer, char *frame);

#endif // SERIAL_CONTROLLER