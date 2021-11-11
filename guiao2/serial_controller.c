#include "serial_controller.h"

int alarmHandlerSet = 0, timeOut;

void alarmCall() { timeOut = 1; }

int openSerial(char *path, struct termios *oldConfig) {

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

int closeSerial(int fd, struct termios oldConfig) {

  // Recovering old config
  if (loadConfig(fd, &oldConfig) != 0) return 1;

  // Closing file descriptor
  if (close(fd) != 0) return 1;

  return 0;
}

SUFrameState SUFrameStateMachine(SUFrameState currentState, char byte) {

  static char address, control;

  switch (currentState) {
  case START:
    if (byte == FLAG) return FLAG_RCV;
    break;
  case FLAG_RCV:
    if (byte == A_EMITTER_RECEIVER) {
      address = byte;
      return A_RCV;
    }
    break;
  case A_RCV:
    if (IS_CONTROL_BYTE(byte)) {
      control = byte;
      return C_RCV;
    }
    break;
  case C_RCV:
    if (byte == BCC(address, control)) return BCC_RCV;
    break;
  case BCC_RCV:
    if (byte == FLAG) return STOP;
    break;
  case STOP:
    return STOP;
  }

  return ERROR;
}

int receiveFrame(int fd, int timer, char *frame) {

  // Initializing alarm
  if (!alarmHandlerSet) {
    signal(SIGALRM, alarmCall);
    alarmHandlerSet = 1;
  }

  char byte;
  SUFrameState state = START;
  int n, idx = 0;

  // Setting alarm
  alarm(timer);
  timeOut = 0;

  while (!timeOut && state != STOP) {
    n = read(fd, &byte, 1);

    if (n < 0) {          // Error on read()
      alarm(0);
      return 1;
    } else if (n == 0) {  // Nothing to read
      continue;
    } else if (n == 1) {  // Read one byte
      state = SUFrameStateMachine(state, byte);  // TODO: adpatar para tramas de informação
      frame[idx++] = byte;

      printf("0x%X\n", byte);   // Debug
    }
  }

  alarm(0);

  // If timed-out
  if (timeOut) return 1;
  
  return 0;
}
