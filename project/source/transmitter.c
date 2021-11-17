#include "../headers/transmitter.h"
#include "../headers/protocol.h"
#include <stdio.h>
#include <unistd.h>

int connectTransmitter(int port) {
    int fd = openSerial(port);
    if (fd < 0) return -1;

    char response[5], data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_EMITTER_RECEIVER, C_SET), FLAG};

    // Sending SET frame and waiting for ACK
    if (communicateFrame(fd, 3, 3, data, 5, response) != 0) return -1;
    if (response[2] != C_UA) return -1;

    return fd;
}

int disconnectTransmitter(int fd) {
    char response[5], data[5] = {FLAG, A_EMITTER_RECEIVER, C_DISC, BCC(A_EMITTER_RECEIVER, C_DISC), FLAG};

    // Sending DISC frame, waiting for DISC and sending back ACK
    if (communicateFrame(fd, 3, 3, data, 5, response) != 0) return -1;
    if (response[2] != C_DISC) return -1;
    data[2] = C_UA;
    data[3] = BCC(data[1], data[2]);
    if (transmitFrame(fd, data, 5) < 0) return -1;

    // Closing serial
    if (closeSerial(fd) != 0) return -1;

    return 1;
}

int communicateFrame(int fd, int attempts, int timer, char *data, int size, char *response) {
  for (int attempt = 1; attempt <= attempts; ++attempt) {

    if (attempt == 0) printf("> Sending frame...\n");
    else printf("> Trying to send it again...\n");

    if (transmitFrame(fd, data, size) < 0) continue;
    return receiveFrame(fd, timer, response);
  }

  return -1;
}
