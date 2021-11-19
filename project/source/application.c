#include "../headers/application.h"
#include "../headers/transmitter.h"
#include "../headers/receiver.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s [0-Transmitter OR 1-Receiver] [Port Number]\n", argv[0]);
        return -1;
    }

    ApplicationStatus status = atoi(argv[1]);       // TODO: Change atoi() to strtoi() to check for errors
    int port = atoi(argv[2]);

    int fd;
    if ((fd = llopen(port, status)) < 0) return -1;

    if (llclose(fd, status) < 0) return -1;

    return 0;
}

int llopen(int port, ApplicationStatus status) {
    return status == TRANSMITTER ? connectTransmitter(port) : connectReceiver(port);
}

int llwrite(int fd, char *buffer, int length) {
    return transmitPacket(fd, buffer, length);  // TODO: Format packet to be sent
}

int llread(int fd, char *buffer) {
    return recievePacket(fd, buffer);           // TODO: Check packet format
}

int llclose(int fd, ApplicationStatus status) {
    return status == TRANSMITTER ? disconnectTransmitter(fd) : disconnectReceiver(fd);
}