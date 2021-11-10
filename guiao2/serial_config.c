#include "serial_config.h"

int getConfig(int fd, struct termios *config) {
  if (tcgetattr(fd, config) == -1) {
    perror("tcgetattr");
    return 1;
  }
  return 0;
}

int loadConfig(int fd, struct termios *config) {
  if (tcsetattr(fd, TCSANOW, config) == -1) {
    perror("tcsetattr");
    return 1;
  }
  return 0;
}

void configNonCanonical(struct termios *config) {
  bzero(config, sizeof(*config));
  config->c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  config->c_iflag = IGNPAR;
  config->c_oflag = 0;

  config->c_lflag = 0;

  config->c_cc[VTIME]    = 1;   /* inter-character timer unused */
  config->c_cc[VMIN]     = 0;   /* blocking read until 1 char received */
}
