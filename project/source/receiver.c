#include "../headers/receiver.h"
#include "../headers/protocol.h"
#include "../headers/auxiliary.h"
#include <stdio.h>
#include <string.h>

int connectReceiver(int port) {
  int fd = openSerial(port);
  if (fd < 0) return -1;

  // Recieving data
  char frame[SU_FRAME_SIZE];
  #ifdef DEBUG
    printf("> Awaiting SET frame to be recieved\n");
  #endif
  if (receiveFrame(fd, 0, frame) != SU_FRAME_SIZE) return -1;

  // Verifying frame
  if (frame[2] != C_SET) return -1;

  // Formatting frame to be sent
  createSUFrame(frame, C_UA);

  // Sending frame
  #ifdef DEBUG
    printf("> Sending UA reponse\n");
  #endif
  if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;

  return fd;
}

int disconnectReceiver(int fd) {
  // Receiving DISC
  char frame[SU_FRAME_SIZE];
  #ifdef DEBUG
    printf("> Awaiting reception of DISC\n");
  #endif
  if (receiveFrame(fd, 0, frame) != SU_FRAME_SIZE) return -1;

  // Verifying frame
  if (frame[2] != C_DISC) return -1;

  // Sending same frame
  #ifdef DEBUG
    printf("> Sending DISC response\n");
  #endif
  if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;

  // Waiting for ACK
  #ifdef DEBUG
    printf("> Awaiting reception of UA\n");
  #endif
  if (receiveFrame(fd, 0, frame) != SU_FRAME_SIZE) return -1;
  if (frame[2] != C_UA) return -1;

  // Closing serial
  if (closeSerial(fd) != 0) return -1;
  
  return 0;
}

int recievePacket(int fd, int attempts, int timer, char *packet) {
  char frame[MAX_FRAME_SIZE];
  int len;

  for (int attempt = 1; attempt <= attempts; ++attempt) {

    // Awaiting reception of frame
    if ((len = receiveFrame(fd, timer, frame)) < 0) {
      createSUFrame(frame, C_REJ(frame[2] >> 7));
      if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;
      continue;
    }

    // Checking control byte
    if (!IS_I_CONTROL_BYTE(frame[2])) {
      createSUFrame(frame, C_REJ(frame[2] >> 7));
      if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;
      continue;
    }

    // Copying array and changing length accordingly
    len -= 6;
    memcpy(packet, frame + 4, len);

    // Creating response
    createSUFrame(frame, C_RR(frame[2] >> 7));

    // Sendind response
    if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;

    return len;    
  }

  return -1;
}
