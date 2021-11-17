#include "../headers/receiver.h"
#include "../headers/protocol.h"
#include <stdio.h>
#include <unistd.h>

int connectReceiver(int port) {
  int fd = openSerial(port);
  if (fd < 0) return -1;

  // Recieving data
  char data[5];
  printf("> Waiting for frame to be recieved...\n");
  if (receiveFrame(fd, 0, data) != 0) return -1;

  // Verifying frame
  if (data[2] != C_SET) return -1;

  // Formatting frame to be sent
  data[2] = C_UA;
  data[3] = BCC(data[1], data[2]);

  // Sending frame
  printf("> Sending response...\n");
  if (write(fd, data, 5) < 0) return -1;
  
  return fd;
}

int disconnectReceiver(int fd) {
  // Receiving DISC
  char data[5];
  printf("> Waiting for frame to be recieved...\n");
  if (receiveFrame(fd, 0, data) != 0) return -1;

  // Verifying frame
  if (data[2] != C_DISC) return -1;

  // Sending same frame
  printf("> Sending response...\n");
  if (write(fd, data, 5) < 0) return -1;

  // Waiting for ACK
  printf("> Waiting for final ACK to be recieved (DISC)...\n");
  if (receiveFrame(fd, 0, data) != 0) return -1;
  if (data[2] != C_UA) return -1;

  // Closing serial
  if (closeSerial(fd) != 0) return -1;
  
  return 1;
}
