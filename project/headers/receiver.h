#ifndef RECEIVER_H
#define RECEIVER_H

int connectReceiver(int port);

int disconnectReceiver(int fd);

int recievePacket(int fd, int attempts, int timer, char *packet);

#endif // RECEIVER_H
