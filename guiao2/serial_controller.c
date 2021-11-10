#include "serial_controller.h"

SUFrameState SUFrameStateMachine(SUFrameState currentState, char byte) {
  static char address;

  switch (currentState) {
  case START:
    if (byte == FLAG) return FLAG_RCV;
    return START;
  case FLAG_RCV:
    address = byte;
    if (byte == A_EMITTER_RECEIVER) return A_RCV;
    else if (byte == FLAG) return FLAG_RCV;
    return START;
  case A_RCV:
    control = byte;
    if (IS_CONTROL_BYTE(byte)) return C_RCV;
    else if (byte == FLAG) return FLAG_RCV;
    return START;
  case C_RCV:
    if (byte == BCC(address, control)) return BCC_RCV;
    else if (byte == FLAG) return FLAG;
    return START;
  case BCC_RCV:
    if (byte == FLAG) return STOP;
    return START;
  case STOP:
    return STOP;
  default:
    return START;
  }
}

int receiveFrame(int fd, int timer, char *frame) {
  
}
