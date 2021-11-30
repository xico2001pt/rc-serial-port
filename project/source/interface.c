#include "../headers/interface.h"
#include "../headers/transmitter.h"
#include "../headers/receiver.h"

static ApplicationStatus applicationStatus;

int llopen(int port, ApplicationStatus status) {
    applicationStatus = status;
    return applicationStatus == TRANSMITTER ? connectTransmitter(port) : connectReceiver(port);
}

int llwrite(int fd, char *buffer, int length) {
    if (applicationStatus == RECEIVER) return -1;
    return transmitPacket(fd, 3, 3, buffer, length);
}

int llread(int fd, char *buffer) {
    if (applicationStatus == TRANSMITTER) return -1;
    return recievePacket(fd, 3, 20, buffer);
}

int llclose(int fd) {
    return applicationStatus == TRANSMITTER ? disconnectTransmitter(fd) : disconnectReceiver(fd);
}