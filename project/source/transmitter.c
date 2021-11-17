#include "transmitter.h"
#include "protocol.h"

int setTransmitterConnection(int port) {
    int fd = openSerial(MODEMDEVICE);
    if (fd < 0) return -1;

    char response[5], data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_EMITTER_RECEIVER, C_SET), FLAG};

    // Sending SET frame and waiting for ACK
    if (communicateFrame(fd, 3, 3, data, 5, response) != 0) return -1;
    if (response[2] != C_UA) return -1;

    return fd;
}

int communicateFrame(int fd, int attempts, int timer, char *data, int size, char *response) {
  for (int attempt = 0; attempt <= attempts; ++attempt) {

    if (attempt == 0) printf("> Sending frame...\n");
    else printf("> Trying to send it again...\n");

    if (write(fd, data, size) < size) continue;
    if (receiveFrame(fd, timer, response) == 0) return 0;
  }

  return 1;
}