#ifndef INTERFACE_H
#define INTERFACE_H

typedef enum { TRANSMITTER, RECEIVER } ApplicationStatus;

/**
 * @brief Opens the connection of the serial port port given with the role/status supplied
 * 
 * @param port      Number of the serial port to be open
 * @param status    Status of the application (TRANSMITTER | RECEIVER)
 * @return int      File descriptor of the data link or -1 on error
 */
int llopen(int port, ApplicationStatus status);

/**
 * @brief Sends a packet to the data link fd given
 * 
 * @param fd        File descriptor of the data link
 * @param buffer    Array of bytes to be written
 * @param length    Length of the array of bytes to be written
 * @return int      Number of written bytes or -1 on error
 */
int llwrite(int fd, char *buffer, int length);

/**
 * @brief Reads a packet from the data link fd given
 * 
 * @param fd        File descriptor of the data link
 * @param buffer    Array of bytes to be written
 * @return int      Number of read bytes or -1 on error
 */
int llread(int fd, char *buffer);

/**
 * @brief Closes the connection of the serial port
 * 
 * @param fd        File descriptor of the data link
 * @return int      Positive value on success or negative value on error
 */
int llclose(int fd);

#endif // INTERFACE_H