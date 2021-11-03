#include "serial_config.h"
#include "protocol.h"
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

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

  // Writting data
  char data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_RECEIVER_EMITTER, C_SET), FLAG};
  write(fd, data, 5);

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);

  return 0;
}
