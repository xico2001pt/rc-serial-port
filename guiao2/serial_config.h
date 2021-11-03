#ifndef SERIAL_CONFIG
#define SERIAL_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

#define BAUDRATE B38400

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_RCV, STOP} ReceptionState;

int getConfig(int fd, struct termios *config);
int loadConfig(int fd, struct termios *config);
void configNonCanonical(struct termios *config);

ReceptionState receptionStateMachine(ReceptionState currentState, char byte);

#endif // SERIAL_CONFIG