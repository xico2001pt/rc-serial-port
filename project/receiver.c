#include "serial_config.h"
#include "protocol.h"
#include "serial_controller.h"

#define MODEMDEVICE "/dev/ttyS11"

int main() {

  struct termios oldConfig;
  int fd = openSerial(MODEMDEVICE, &oldConfig);
  if (fd < 0) return 1;

  int stop = 0;
  char data[512];

  while (!stop) {

    // Recieving data
    printf("> Waiting for frame to be recieved...\n");
    if (receiveFrame(fd, 0, data) != 0) return 1;

    // Formatting frame to be sent
    if (data[2] == C_SET) data[2] = C_UA;
    else if (data[2] == C_DISC) stop = 1;
    else if (data[2] == C_I(0)) {
      for (int i = 4; i < 9; ++i) printf("%c", data[i]);
      data[2] = C_RR(0);
      data[4] = FLAG;
    }
    else return 1;                        // TODO: Change

    data[3] = BCC(data[1], data[2]);

    // Sending frame
    printf("> Sending response...\n");
    if (write(fd, data, 5) < 0) return 1;
  }

  // Waiting for ACK
  printf("> Waiting for final ACK to be recieved (DISC)...\n");
  if (receiveFrame(fd, 0, data) != 0) return 1;
  if (data[2] != C_UA) return 1;

  // Closing serial
  if (closeSerial(fd, oldConfig) != 0) return 1;
  
  return 0;
}
