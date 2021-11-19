#include "../headers/auxiliary.h"
#include "../headers/protocol.h"
#include <string.h>

void createSUFrame(char *frame, char control) {
  frame[0] = FLAG;
  frame[1] = A_TRANSMITTER_RECEIVER;
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
