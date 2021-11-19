#ifndef RECEIVER_H
#define RECEIVER_H

int connectReceiver(int port);

int disconnectReceiver(int fd);

int recievePacket(int fd, char *packet);

#endif // RECEIVER_H
