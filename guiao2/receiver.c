#include "serial_config.h"
#include "protocol.h"
#include "serial_controller.h"

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
  SUFrameState state = START;
  char byte;

  while (state != STOP) {
    read(fd, &byte, 1);
    fprintf(stderr, "0x%X\n", byte);
    state = SUFrameStateMachine(state, byte);
  }

  char data[5] = {FLAG, A_EMITTER_RECEIVER, 0, 0, FLAG};
  if (control == C_SET) data[2] = C_UA;
  else if (control == C_DISC) data[2] = C_DISC;
  else exit(1);

  data[3] = BCC(data[1], data[2]);

  write(fd, data, 5);

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);
  
  return 0;
}
