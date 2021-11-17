#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>

static struct termios *oldConfig;

int getConfig(int fd, struct termios *config) {
  if (tcgetattr(fd, config) == -1) {
    perror("tcgetattr");
    return 1;
  }
  return 0;
}

int loadConfig(int fd, struct termios *config) {
  if (tcsetattr(fd, TCSANOW, config) == -1) {
    perror("tcsetattr");
    return 1;
  }
  return 0;
}

void configNonCanonical(struct termios *config) {
  bzero(config, sizeof(*config));
  config->c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  config->c_iflag = IGNPAR;
  config->c_oflag = 0;

  config->c_lflag = 0;

  config->c_cc[VTIME]    = 1;   /* inter-character timer unused */
  config->c_cc[VMIN]     = 0;   /* blocking read until 1 char received */
}

int alarmHandlerSet = 0, timeOut;

void alarmCall() { timeOut = 1; }

int openSerial(char *path) {

  // Opening file descriptor
  int fd = open(path, O_RDWR | O_NOCTTY);
  if (fd < 0) return -1;

  // Saving old config
  if (getConfig(fd, oldConfig) != 0) return -1;

  // Getting new config
  struct termios newConfig;
  configNonCanonical(&newConfig);

  // Flushing unread or not written data of SerialPort
  tcflush(fd, TCIOFLUSH);

  // Loading new config
  if (loadConfig(fd, &newConfig) != 0) return -1;

  return fd;
}

int closeSerial(int fd) {

  // Recovering old config
  if (loadConfig(fd, oldConfig) != 0) return 1;

  // Closing file descriptor
  if (close(fd) != 0) return 1;

  return 0;
}

int receiveFrame(int fd, int timer, char *frame) {

  // Initializing alarm
  if (!alarmHandlerSet) {
    signal(SIGALRM, alarmCall);
    alarmHandlerSet = 1;
  }

  char byte;
  FrameState state = START;
  int n, idx = 0;

  // Setting alarm
  alarm(timer);
  timeOut = 0;

  while (!timeOut && state != STOP) {
    printf("%d", state);
    n = read(fd, &byte, 1);

    if (n < 0) {          // Error on read()
      alarm(0);
      return 1;
    } else if (n == 0) {  // Nothing to read
      continue;
    } else if (n == 1) {  // Read one byte
      state = FrameStateMachine(state, byte);
      frame[idx++] = byte;

      printf("0x%X\n", byte);   // Debug
    }
  }

  alarm(0);

  // If timed-out
  if (timeOut) return 1;
  
  return 0;
}

FrameState FrameStateMachine(FrameState currentState, char byte) {
  static char address, control, dataBCC;

  switch (currentState) {
  case START:
    if (byte == FLAG) return FLAG_RCV;
    break;
  case FLAG_RCV:
    if (byte == A_EMITTER_RECEIVER || byte == A_RECEIVER_EMITTER) {
      address = byte;
      return A_RCV;
    }
    break;
  case A_RCV:
    if (IS_SU_CONTROL_BYTE(byte) || IS_I_CONTROL_BYTE(byte)) {
      control = byte;
      return C_RCV;
    }
    break;
  case C_RCV:
    if (byte == BCC(address, control)) {
      if (IS_I_CONTROL_BYTE(control)) {
        dataBCC = 0;
        return BCC1_RCV;
      }
      return BCC_RCV;
    }
    break;
  case BCC_RCV:
    if (byte == FLAG) return STOP;
    break;
  case BCC1_RCV:
    if (byte == FLAG) {
      if (dataBCC == 0) return STOP;
    } else {
      dataBCC = BCC(dataBCC, byte);
      return BCC1_RCV;
    }
    break;
  case STOP:
    return STOP;
  }

  return ERROR;
}
