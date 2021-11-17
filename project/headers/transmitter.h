#ifndef TRANSMITTER
#define TRANSMITTER

int setTransmitterConnection(int port);

int disconnectTransmitterConnection();

// Auxiliary Functions

int communicateFrame(int fd, int attempts, int timer, char *data, int size, char *response);

#endif // TRANSMITTER