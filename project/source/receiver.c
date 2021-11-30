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
  if (receiveFrame(fd, 20, frame) != SU_FRAME_SIZE) return -1;

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
  static int S = 0;
  char frame[MAX_FRAME_SIZE];
  int len;

  for (int attempt = 1; attempt <= attempts; ++attempt) {

    // Awaiting reception of frame
    if ((len = receiveFrame(fd, timer, frame)) < 0) {
      createSUFrame(frame, C_REJ(S));               // We sent an REJ with the number of the packet that we want to read and discard the frame read
      transmitFrame(fd, frame, SU_FRAME_SIZE);      // We don't need to know if frame was correctly trasmitted or not, either way we want to attempt the read again
      continue;
    }

    // Checking control byte, if it's not an information frame or the frame is a duplicate (occured a timeout in transmitter)
    if (!IS_I_CONTROL_BYTE(frame[2]) || frame[2] == C_I(1 - S)) {
      createSUFrame(frame, C_REJ(S));
      transmitFrame(fd, frame, SU_FRAME_SIZE);
      continue;
    }

    // Copying array and changing length accordingly
    len -= 6;
    memcpy(packet, frame + 4, len);

    // Creating response
    S = 1 - S;
    createSUFrame(frame, C_RR(S));

    // Sendind response
    if (transmitFrame(fd, frame, SU_FRAME_SIZE) < 0) return -1;

    return len;    
  }

  return -1;
}
