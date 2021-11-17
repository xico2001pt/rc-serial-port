#include "serial_config.h"
#include "protocol.h"
#include "serial_controller.h"

#define MODEMDEVICE "/dev/ttyS10"

int communicateFrame(int fd, int attempts, int timer, char *data, char *response) {
  for (int attempt = 0; attempt <= attempts; ++attempt) {

    if (attempt == 0) printf("> Sending frame...\n");
    else printf("> Trying to send it again...\n");

    if (write(fd, data, 5) < 5) continue; // TODO: variable com tamanho
    if (receiveFrame(fd, timer, response) == 0) return 0;
  }

  return 1;
}

int main() {

  struct termios oldConfig;
  int fd = openSerial(MODEMDEVICE, &oldConfig);
  if (fd < 0) return 1;

  char response[5], data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_EMITTER_RECEIVER, C_SET), FLAG};

  // Sending SET frame and waiting for ACK
  if (communicateFrame(fd, 3, 3, data, response) != 0) return 1;
  if (response[2] != C_UA) return 1;

  // Sending DISC frame, waiting for DISC and sending back ACK
  data[2] = C_DISC;
  data[3] = BCC(data[1], data[2]);
  if (communicateFrame(fd, 3, 3, data, response) != 0) return 1;
  if (response[2] != C_DISC) return 1;
  data[2] = C_UA;
  data[3] = BCC(data[1], data[2]);
  if (write(fd, data, 5) < 0) return 1;

  // Closing serial
  if (closeSerial(fd, oldConfig) != 0) return 1;

  return 0;
}
