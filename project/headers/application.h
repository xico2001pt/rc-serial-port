#ifndef APPLICATION_H
#define APPLICATION_H

#include "protocol.h"

#define MAX_PATH_SIZE 100

#define MAX_PACKET_SIZE ((MAX_FRAME_SIZE - 2) / 2 - 4)
#define HEADER_SIZE     4
#define MAX_DATA_SIZE   (MAX_PACKET_SIZE - HEADER_SIZE)

#define C_DATA  1
#define C_START 2
#define C_END   3

#define ABORT(file) {close(file); return -1;}

/**
 * @brief Transmits the file in the given path
 * 
 * @param fd            File descriptor of the data link
 * @param filePath      Path of the file to be transmitted
 * @param pathLength    Length of the file path (bytes)
 * @return int          0 on success, -1 otherwise
 */
int transmitFile(int fd, char *filePath, int pathLength); 

/**
 * @brief Receives a file from the given file descriptor, storing it the same location as the executable
 * 
 * @param fd    File descriptor of the data link
 * @return int  0 on success, -1 otherwise
 */
int receiveFile(int fd);

#endif // APPLICATION_H
