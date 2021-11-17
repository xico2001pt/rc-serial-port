#include "../headers/transmitter.h"
#include "../headers/protocol.h"
#include <stdio.h>
#include <unistd.h>

int connectTransmitter(int port) {
  int fd = openSerial(port);
  if (fd < 0) return -1;

  char frame[SU_FRAME_SIZE];
  createSUFrame(frame, C_SET);
  
  // Sending SET frame and waiting for UA
  if (communicateFrame(fd, 3, 3, frame, SU_FRAME_SIZE) != 0) return -1;
  if (frame[2] != C_UA) return -1;

  return fd;
}

int disconnectTransmitter(int fd) {
  char frame[SU_FRAME_SIZE];
  createSUFrame(frame, C_DISC);

  // Sending DISC frame, waiting for DISC
  if (communicateFrame(fd, 3, 3, frame, SU_FRAME_SIZE) != 0) return -1;
  if (frame[2] != C_DISC) return -1;
  
  // Sending back UA
  createSUFrame(frame, C_UA);
  if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;

  // Closing serial
  if (closeSerial(fd) != 0) return -1;

  return 1;
}

int transmitPacket(int fd, char *packet, int length) {
  static int S = 0;
  char frame[MAX_FRAME_SIZE];
  createIFrame(frame, C_RR(S), packet, length);
  
  if (communicateFrame(fd, 3, 3, frame, length + 6) != SU_FRAME_SIZE) return -1;
  if (frame[2] == C_RR(0)) S = 0;
  else if (frame[2] == C_RR(1)) S = 1;
  else return -1;
  return 0;
}

int communicateFrame(int fd, int attempts, int timer, char *frame, int length) {
  for (int attempt = 1; attempt <= attempts; ++attempt) {

    if (attempt == 0) printf("> Sending frame...\n");
    else printf("> Trying to send it again...\n");

    if (transmitFrame(fd, frame, length) < 0) continue;
    return receiveFrame(fd, timer, frame);
  }

  return -1;
}

void createSUFrame(char *frame, char control) {
  frame[0] = FLAG;
  frame[1] = A_TRANSMITTER_RECEIVER;      // TODO: CTRL-F change to A_TRANSMITTER_RECEIVER
  frame[2] = control;
  frame[3] = BCC(frame[2], frame[3]);
  frame[4] = FLAG;
}

void createIFrame(char *frame, char control, char *data, int length) {
  frame[0] = FLAG;
  frame[1] = A_TRANSMITTER_RECEIVER;
  frame[2] = control;
  frame[3] = BCC(frame[2], frame[3]);
  strncpy(frame + 4, data, length);
  frame[length + 6 - 2] = calculateBCC(data, length);
  frame[length + 6 - 1] = FLAG;
}
