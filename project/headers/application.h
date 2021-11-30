#ifndef APPLICATION_H
#define APPLICATION_H

#define MAX_PATH_SIZE 100

#define MAX_PACKET_SIZE 512
#define HEADER_SIZE     4
#define MAX_DATA_SIZE   (MAX_PACKET_SIZE - HEADER_SIZE)

#define C_DATA  1
#define C_START 2
#define C_END   3

#define ABORT(file) {close(file); return -1;}

int transmitFile(int fd, char *filePath, int pathLength); 

int receiveFile(int fd);

#endif // APPLICATION_H
