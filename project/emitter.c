#include "serial_config.h"
#include "protocol.h"
#include "serial_controller.h"

#define MODEMDEVICE "/dev/ttyS10"

int communicateFrame(int fd, int attempts, int timer, char *data, int size, char *response) {
  for (int attempt = 0; attempt <= attempts; ++attempt) {

    if (attempt == 0) printf("> Sending frame...\n");
    else printf("> Trying to send it again...\n");

    if (write(fd, data, size) < size) continue; // TODO: variable com tamanho
    if (receiveFrame(fd, timer, response) == 0) return 0;
  }

  return 1;
}

char calculateBCC(char *data, int length) {
  char result = 0;
  for (int i = 0; i < length; ++i, ++data) result = BCC(result, *data);
  return result;
}

int main() {

  struct termios oldConfig;
  int fd = openSerial(MODEMDEVICE, &oldConfig);
  if (fd < 0) return 1;

  char response[5], data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_EMITTER_RECEIVER, C_SET), FLAG};

  // Sending SET frame and waiting for ACK
  if (communicateFrame(fd, 3, 3, data, 5, response) != 0) return 1;
  if (response[2] != C_UA) return 1;

  // Sending information frame
  char info[11] = {FLAG, A_EMITTER_RECEIVER, C_I(0), BCC(A_EMITTER_RECEIVER, C_I(0)), 'H', 'E', 'L', 'L', 'O', calculateBCC("HELLO", 5), FLAG};
  if (communicateFrame(fd, 3, 3, info, 11, response) != 0) return 1;
  if (response[2] != C_RR(0)) return 1;

  // Sending DISC frame, waiting for DISC and sending back ACK
  data[2] = C_DISC;
  data[3] = BCC(data[1], data[2]);
  if (communicateFrame(fd, 3, 3, data, 5, response) != 0) return 1;
  if (response[2] != C_DISC) return 1;
  data[2] = C_UA;
  data[3] = BCC(data[1], data[2]);
  if (write(fd, data, 5) < 0) return 1;

  // Closing serial
  if (closeSerial(fd, oldConfig) != 0) return 1;

  return 0;
}