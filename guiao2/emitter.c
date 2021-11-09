#include "serial_config.h"
#include "protocol.h"
#include "state_machine.h"

#define MODEMDEVICE "/dev/ttyS10"

/*static int tries = 0, tryAgain = 1;

void alarmCall() {
  tries++;
  tryAgain = 0;
}*/

int main() {

  int fd, res;
  struct termios oldConfig, newConfig;
  char buf[255];

  // Opening file descriptor
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(MODEMDEVICE);
    exit(1);
  }

  // Saving old config
  if (getConfig(fd, &oldConfig) != 0) exit(1);

  // Getting new config
  configNonCanonical(&newConfig);

  // Flushing unread or not written data of SerialPort
  tcflush(fd, TCIOFLUSH);

  // Loading new config
  if (loadConfig(fd, &newConfig) != 0) exit(1);

  /*// Setting SIGALARM handler
  signal(SIGALRM, alarmCall);

  while (tries < 3) {
    
    // Writing data
    char data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_EMITTER_RECEIVER, C_SET), FLAG};
    write(fd, data, 5);

    // Setting time-out
    alarm(3);

    // Recieving data
    SUFrameState state = START;
    char byte;

    // Making it try again
    tryAgain = 1;

    while (tryAgain && state != STOP) {
      read(fd, &byte, 1);                 // After SIGALARM call, it gets stuck in the read, I don't know any way of fixing this without changing read() parameters (it's a lot of work)
      fprintf(stderr, "0x%X\n", byte);
      state = SUFrameStateMachine(state, byte);
    }

    if (state == STOP) break;
  }

  // Reseting alarm
  alarm(0);*/

  // Writing data
  char data[5] = {FLAG, A_EMITTER_RECEIVER, C_SET, BCC(A_EMITTER_RECEIVER, C_SET), FLAG};
  write(fd, data, 5);

  SUFrameState state = START;
  char byte;

  // Reading data
  while (state != STOP) {
    read(fd, &byte, 1);
    fprintf(stderr, "0x%X\n", byte);
    state = SUFrameStateMachine(state, byte);
  }

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);

  return 0;
}
