#include "serial_config.h"
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

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

  // Reading data
  printf("Waiting for a read...\n");
  res = read(fd, buf, 255);
  printf(":%s:%d\n", buf, res);

  // Writting data
  printf("Sending back the received message...\n");
  write(fd, buf, strlen(buf) + 1);

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);
  
  return 0;
}
