#include "serial_config.h"
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

  // Recieving data from stdin
  printf("> ");
  if (fgets(buf, 255, stdin) == NULL) exit(1);

  // Writting data
  write(fd, buf, strlen(buf) + 1);

  // Reading data
  printf("Waiting to recieve the message back...\n");
  res = read(fd, buf, 255);
  printf(":%s:%d\n", buf, res);

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);

  return 0;
}
