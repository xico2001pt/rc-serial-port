#include "../headers/transmitter.h"
#include "../headers/protocol.h"
#include "../headers/auxiliary.h"
#include <stdio.h>

int connectTransmitter(int port) {
  int fd = openSerial(port);
  if (fd < 0) return -1;

  char frame[SU_FRAME_SIZE];
  createSUFrame(frame, C_SET);
  
  // Sending SET frame and waiting for UA
  if (communicateFrame(fd, 3, 3, frame, SU_FRAME_SIZE) != SU_FRAME_SIZE) return -1;
  if (frame[2] != C_UA) return -1;

  return fd;
}

int disconnectTransmitter(int fd) {
  char frame[SU_FRAME_SIZE];
  createSUFrame(frame, C_DISC);

  // Sending DISC frame, waiting for DISC
  if (communicateFrame(fd, 3, 3, frame, SU_FRAME_SIZE) != SU_FRAME_SIZE) return -1;
  if (frame[2] != C_DISC) return -1;
  
  // Sending back UA
  createSUFrame(frame, C_UA);
  if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;

  // Closing serial
  if (closeSerial(fd) != 0) return -1;

  return 0;
}

int communicateFrame(int fd, int attempts, int timer, char *frame, int length) {

  if (attempts < 1) {
    fprintf(stderr, "communicateFrame(): Invalid number of attempts (>= 1)\n");
    return -1;
  }

  for (int attempt = 1; attempt <= attempts; ++attempt) {

    #ifdef DEBUG
      if (attempt == 1) printf("> Sending frame\n");
      else printf("> Trying to send it again, attempt n = %d\n", attempt);
    #endif

    // Trying to transmit frame
    if (transmitFrame(fd, frame, length) < 0) continue;

    #ifdef DEBUG
      printf("> Awaiting response\n");
    #endif

    // Trying to recieve frame
    int len;
    if ((len = receiveFrame(fd, timer, frame)) != -1) return len;
  }

  return -1;
}

int transmitPacket(int fd, char *packet, int length) {
  static int S = 0;

  // Failsafe
  if (length + 6 > MAX_FRAME_SIZE) {
    fprintf(stderr, "transmitPacket(): Frame incapable of storing packet (default size = 1030 bytes) (Frame can store up to (1030-6)/2 = 512 bytes of data safely!)\n");
    return -1;
  }

  char frame[MAX_FRAME_SIZE];

  // Creating the frame to be sent
  createIFrame(frame, C_RR(S), packet, length);
  
  // Sending frame and receiving a response
  if (communicateFrame(fd, 3, 3, frame, length + 6) != SU_FRAME_SIZE) return -1;

  // Checking control byte and changing S if it's in conformity
  if (frame[2] == C_RR(0)) S = 0;
  else if (frame[2] == C_RR(1)) S = 1;
  else return -1;

  return 0;
}
