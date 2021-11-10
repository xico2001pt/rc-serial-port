#include "serial_config.h"
#include "protocol.h"
#include "serial_controller.h"

#define MODEMDEVICE "/dev/ttyS10"

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

  establishConnection(fd);

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);

  return 0;
}

int establishConnection(int fd) {
  char status[5], data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_EMITTER_RECEIVER, C_SET), FLAG};
  return communicateFrame(fd, 3, 3, data, status, 1);
}