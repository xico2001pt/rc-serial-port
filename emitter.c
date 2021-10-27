#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS10"
#define _POSIX_SOURCE 1

int main() {

  int fd;
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

  // Recieving data from stdin
  printf("> ");
  if (fgets(buf, 255, stdin) == NULL) {
      exit(1);
  }

  // Writting data
  write(fd, buf, strlen(buf) + 1);

  // Recovering old config
  loadConfig(fd, &oldConfig);

  // Closing file descriptor
  close(fd);
  
  return 0;
}

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

    config->c_cc[VTIME]    = 0;   /* inter-character timer unused */
    config->c_cc[VMIN]     = 1;   /* blocking read until 1 char received */
}
