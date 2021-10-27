#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#define BAUDRATE B38400

int main(int argc, char** argv) {

  int fd;
  struct termios oldConfig, newConfig;
  char buf[255];

  // Verifying input
  if (argc < 2 || (strcmp("/dev/ttyS0", argv[1]) != 0 && strcmp("/dev/ttyS1", argv[1]) != 0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  // Opening file descriptor
  if (fd = open(argv[1], O_RDWR | O_NOCTTY) < 0) {
    perror(argv[1]);
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
  if (fgets(buf, 255, stdin) == NULL) {
      exit(1);
  }

  // Writting data
  write(fd, buf, 255);

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
    config->c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
}
