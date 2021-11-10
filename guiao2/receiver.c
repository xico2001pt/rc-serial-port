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

  printf("> Waiting for frame to be recieved...\n");

  // Recieving data
  char data[5];
  if (receiveFrame(fd, 0, data) == 1) exit(1);

  //sleep(4);

  // Formatting frame to be sent
  if (data[2] == C_SET) data[2] = C_UA;
  else if (data[2] == C_DISC) data[2] = C_DISC;
  else exit(1);
  data[3] = BCC(data[1], data[2]);

  // Sending frame
  write(fd, data, 5);

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);
  
  return 0;
}

int establishConnection(int fd) {
  
}