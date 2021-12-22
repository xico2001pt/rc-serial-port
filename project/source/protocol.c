#include "../headers/protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static struct termios oldConfig;
int alarmHandlerSet = 0, timeOut;

void alarmCall() { timeOut = 1; }

int getConfig(int fd, struct termios *config) {
  if (tcgetattr(fd, config) == -1) {
    perror("tcgetattr");
    return -1;
  }
  return 0;
}

int loadConfig(int fd, struct termios *config) {
  if (tcsetattr(fd, TCSANOW, config) == -1) {
    perror("tcsetattr");
    return -1;
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
  config->c_cc[VMIN]     = 0;   /* blocking read until 0 char received */
}

int openSerial(int port) {
  char path[20];
  snprintf(path, 20, "/dev/ttyS%d", port);

  // Opening file descriptor
  int fd = open(path, O_RDWR | O_NOCTTY);
  if (fd < 0) return -1;

  // Saving old config
  if (getConfig(fd, &oldConfig) != 0) return -1;

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
  if (loadConfig(fd, &oldConfig) != 0) return -1;

  // Closing file descriptor
  if (close(fd) != 0) return -1;

  return 0;
}

int transmitFrame(int fd, char *frame, int length) {

  #ifdef DEBUG
    printf("Transmitting:");
    for (int n = 0; n < length; ++n) printf(" 0x%02hhX", frame[n]);
    printf("\n");
  #endif

  char frameCopy[MAX_FRAME_SIZE];
  memcpy(frameCopy, frame, length);

  int len = length;
  if (IS_I_CONTROL_BYTE(frameCopy[2])) {
    len = stuffing(frameCopy + 1, length - 2) + 2;
    frameCopy[len - 1] = FLAG;
  }

  if (write(fd, frameCopy, len) < 0) return -1;
  
  return 0;
}

int receiveFrame(int fd, int timer, char *frame) {

  // Initializing alarm
  if (!alarmHandlerSet) {
    signal(SIGALRM, alarmCall);
    alarmHandlerSet = 1;
  }

  FrameState state = START;
  char byte;
  int n, idx = 0;

  // Setting alarm
  alarm(timer);
  timeOut = 0;

  while (!timeOut && state != STOP) {
    n = read(fd, &byte, 1);
    
    if (n < 0) {          // Error on read()
      alarm(0);
      return -1;
    } else if (n == 0) {  // Nothing to read
      continue;
    } else if (n == 1) {  // Read one byte
      frame[idx++] = byte;
      if ((state = FrameStateMachine(state, frame, &idx)) == ERROR) {   // If BCC2 fails
        tcflush(fd, TCIOFLUSH);
        alarm(0);
        return -1;
      }
    }
  }

  alarm(0);

  // If timed-out or final state isn't STOP
  if (timeOut || state != STOP) {
    tcflush(fd, TCIFLUSH);
    return -1;
  }

  #ifdef DEBUG
    printf("Recieved:    ");
    for (int n = 0; n < idx; ++n) printf(" 0x%02hhX", frame[n]);
    printf("\n");
  #endif
  
  return idx;
}

FrameState FrameStateMachine(FrameState currentState, char *frame, int *length) {
  char byte = frame[*length - 1];

  switch (currentState) {
  case START:
    if (byte == FLAG) return FLAG_RCV;
    break;
  case FLAG_RCV:
    if (byte == A_TRANSMITTER_RECEIVER || byte == A_RECEIVER_TRANSMITTER) return A_RCV;
    break;
  case A_RCV:
    if (IS_SU_CONTROL_BYTE(byte) || IS_I_CONTROL_BYTE(byte)) return C_RCV;
    break;
  case C_RCV:
    if (byte == BCC(frame[1], frame[2])) {
      if (IS_I_CONTROL_BYTE(frame[2])) return BCC1_RCV;
      return BCC_RCV;
    }
    break;
  case BCC_RCV:
    if (byte == FLAG) return STOP;
    break;
  case BCC1_RCV:
    if (byte == FLAG) {
      int len = destuffing(frame + 1, *length - 2) + 2;
      frame[len - 1] = FLAG;
      *length = len;
      if (calculateBCC(frame + 4, len - 6) == frame[len - 2]) return STOP;
      else ERROR;                            // If BCC2 fails
    }
    return BCC1_RCV;
  case STOP:
    return STOP;
  }

  return START;
}

char calculateBCC(char *data, int length) {
  char result = 0;
  for (int i = 0; i < length; ++i, ++data) result = BCC(result, *data);
  return result;
}

int destuffing(char *data, int length) {
  int size = 0;
  for (int x = 0; x < length; ++x, ++size) {
    if (data[x] == ESCAPE && data[x+1] == FLAG_STUFFING) {
      data[size] = FLAG;
      x += 1;
    } else if (data[x] == ESCAPE && data[x+1] == ESCAPE_STUFFING) {
      data[size] = ESCAPE;
      x += 1;
    } else {
      data[size] = data[x];
    }
  }
  return size;
}

int stuffing(char *data, int length) {
  char aux[MAX_FRAME_SIZE];
  memcpy(aux, data, length);
  int size = 0;
  for (int x = 0; x < length; x++) {
    if (aux[x] == FLAG) {
      data[size++] = ESCAPE;
      data[size++] = FLAG_STUFFING;
    } else if (aux[x] == ESCAPE) {
      data[size++] = ESCAPE;
      data[size++] = ESCAPE_STUFFING;
    }
    else data[size++] = aux[x];
  }
  return size;
}

void createSUFrame(char *frame, char control) {
  frame[0] = FLAG;
  frame[1] = A_TRANSMITTER_RECEIVER;
  frame[2] = control;
  frame[3] = BCC(frame[1], frame[2]);
  frame[4] = FLAG;
}

void createIFrame(char *frame, char control, char *data, int length) {
  frame[0] = FLAG;
  frame[1] = A_TRANSMITTER_RECEIVER;
  frame[2] = control;
  frame[3] = BCC(frame[1], frame[2]);
  memcpy(frame + 4, data, length);
  frame[length + 6 - 2] = calculateBCC(data, length);
  frame[length + 6 - 1] = FLAG;
}
