#include "../headers/application.h"
#include "../headers/transmitter.h"
#include "../headers/receiver.h"

// TESTING
#include "../headers/protocol.h"
#include <stdio.h>

int main(int argc, char **argv) {
    // if argv[1] == 0, então é transmitter
    // senão, é o receiver
    //llopen();

    char data[200] = {FLAG, FLAG, 0x7e, 0x5e, 0x7d, 0x5e, 0x7e};
    for (int n = 0; n < 7; n++) printf("%X ", data[n]);
    printf("\n");
    int len = stuffing(data, 7);
    for (int n = 0; n < len; n++) printf("%X ", data[n]);
    printf("\n");
    len = destuffing(data, len);
    printf("%d\n", len);
    for (int n = 0; n < len; n++) printf("%X ", data[n]);
    printf("\n");
}

int llopen(int port, ApplicationStatus status) {
    return status == TRANSMITTER ? connectTransmitter(port) : connectReceiver(port);
}

int llwrite(int fd, char *buffer, int length) {

}

int llread(int fd, char *buffer) {

}

int llclose(int fd, ApplicationStatus status) {
    return status == TRANSMITTER ? disconnectTransmitter(fd) : disconnectReceiver(fd);
}