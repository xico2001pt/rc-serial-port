#ifndef SERIAL_CONFIG
#define SERIAL_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <signal.h>

#define BAUDRATE B38400

int getConfig(int fd, struct termios *config);
int loadConfig(int fd, struct termios *config);
void configNonCanonical(struct termios *config);

#endif // SERIAL_CONFIG