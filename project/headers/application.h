#ifndef APPLICATION_H
#define APPLICATION_H

typedef enum { TRANSMITTER, RECEIVER } ApplicationStatus;

#define MAX_PACKET_SIZE 512
#define HEADER_SIZE     4
#define MAX_DATA_SIZE   (MAX_PACKET_SIZE - HEADER_SIZE)

#define C_DATA  1
#define C_START 2
#define C_END   3

#define T_FILE_SIZE 0
#define T_FILE_NAME 1

/*typedef struct {
    int fileDescriptor;
    int status;
} ApplicationLayer;*/

/**
 * @brief 
 * 
 * @param port      Number of the serial port to be open
 * @param status    Status of the application (TRANSMITTER | RECEIVER)
 * @return int      File descriptor of the data link or -1 on error
 */
int llopen(int port, ApplicationStatus status);

/**
 * @brief 
 * 
 * @param fd        File descriptor of the data link
 * @param buffer    Array of bytes to be written
 * @param length    Length of the array of bytes to be written
 * @return int      Number of written bytes or -1 on error
 */
int llwrite(int fd, char *buffer, int length);

/**
 * @brief 
 * 
 * @param fd        File descriptor of the data link
 * @param buffer    Array of bytes to be written
 * @return int      Number of read bytes or -1 on error
 */
int llread(int fd, char *buffer);

/**
 * @brief 
 * 
 * @param fd        File descriptor of the data link
 * @param status    Status of the application (TRANSMITTER | RECEIVER)          // TODO: Remove this? We think we need this for this to work
 * @return int      Positive value on success or negative value on error
 */
int llclose(int fd, ApplicationStatus status);

#endif // APPLICATION_H
