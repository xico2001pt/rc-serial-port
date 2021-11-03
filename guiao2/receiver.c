#include "serial_config.h"
#include "protocol.h"

#define MODEMDEVICE "/dev/ttyS11"

int main() {

  int fd, res;
  struct termios oldConfig, newConfig;
  char buf[255];

  // Opening file descriptor
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(MODEMDEVICE);
    exit(1);
  }

  // Saving old config
  if (getConfig(fd, &oldConfig) != 0) exit(1);

  // Getting new config
  configNonCanonical(&newConfig);

  // Flushing unread or not written data of SerialPort
  tcflush(fd, TCIOFLUSH);

  // Loading new config
  if (loadConfig(fd, &newConfig) != 0) exit(1);

  // Recieving data
  ReceptionState state = START;
  char byte, address, control, n;

  while (state != STOP) {
    n = read(fd, &byte, 1);
    fprintf(stderr, "%d\n", n);
    fprintf(stderr, "0x%X\n", byte);
    state = receptionStateMachine(state, byte);

    if (state == A_RCV) address = byte;
    else if (state == C_RCV) {
      control = byte;
      if (read(fd, &byte, 1) != 1) break;
      if (byte == BCC(address, control)) state = BCC_RCV;
      else if (byte == FLAG) state = FLAG;
      else state = START;
    }
  }

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);
  
  return 0;
}

ReceptionState receptionStateMachine(ReceptionState currentState, char byte) {
  switch (currentState) {
  case START:
    if (byte == FLAG) return FLAG_RCV;
    return START;
  case FLAG_RCV:
    if (byte == A_EMITTER_RECEIVER) return A_RCV;
    else if (byte == FLAG) return FLAG_RCV;
    return START;
  case A_RCV:
    if (byte == C_SET) return C_RCV;
    else if (byte == FLAG) return FLAG_RCV;
    return START;
  case BCC_RCV:
    if (byte == FLAG) return STOP;
    return START;
  case STOP:
    return STOP;
  default:
    return START;
  }
}
