#include "serial_controller.h"

SUFrameState SUFrameStateMachine(SUFrameState currentState, char byte) {
  static char address;

  switch (currentState) {
  case START:
    if (byte == FLAG) return FLAG_RCV;
    break;
  case FLAG_RCV:
    if (byte == A_EMITTER_RECEIVER) {
      address = byte;
      return A_RCV;
    }
    break;
  case A_RCV:
    if (IS_CONTROL_BYTE(byte)) {
      control = byte;
      return C_RCV;
    }
    break;
  case C_RCV:
    if (byte == BCC(address, control)) return BCC_RCV;
    break;
  case BCC_RCV:
    if (byte == FLAG) return STOP;
    break;
  case STOP:
    return STOP;
  }

  return ERROR;
}

/**
 * @param timer: Number of coiso que coiso
 */
int receiveFrame(int fd, int timer, char *frame) {
  char byte;
  SUFrameState state = START;
  int n, idx = 0;
  
  while (state != STOP) {
    n = read(fd, &byte, 1);
    printf("hello there\n");
    if (n != 1) {
      timer -= 1;  // TODO: CHANGE TO MACRO
      if (timer <= 0) return 1;
    } else {
      frame[idx++] = byte;
      fprintf(stderr, "0x%X, %d\n", byte, n);
      state = SUFrameStateMachine(state, byte);
      if (state == ERROR) return 1;
    }
  }
  
  return 0;
}
