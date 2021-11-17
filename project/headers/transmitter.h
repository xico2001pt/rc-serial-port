#ifndef TRANSMITTER_H
#define TRANSMITTER_H

int connectTransmitter(int port);

int disconnectTransmitter(int fd);

// Auxiliary Functions

int communicateFrame(int fd, int attempts, int timer, char *data, int size, char *response);

#endif // TRANSMITTER_H