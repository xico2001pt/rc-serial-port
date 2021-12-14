#ifndef TRANSMITTER_H
#define TRANSMITTER_H

/**
 * @brief Settles the connection between transmitter and receiver
 * 
 * @param port  Number of port of the serial port
 * @return int  Returns -1 on error, fd of serial port when it's successful
 */
int connectTransmitter(int port);

/**
 * @brief Disconnects the connection between transmitter and receiver
 * 
 * @param fd    File descriptor of the serial port
 * @return int  Returns -1 on error, 0 when it's successful
 */
int disconnectTransmitter(int fd);

/**
 * @brief Tries to send the frame the given times to the receiver. It tries again if either nothing is received after the given seconds or the transmission fails
 * 
 * @param fd        File descriptor of the serial port
 * @param attempts  Number of attempts (>= 1)
 * @param timer     Time in seconds for time out (0 means that it'll never time out)
 * @param frame     Array that contains the data to be written and the response afterwards
 * @param size      Size in bytes of the array to be sent
 * @return int      Returns -1 on error and the length of the response when it's successful
 */
int communicateFrame(int fd, int attempts, int timer, char *frame, int size);

/**
 * @brief Formats the packet into a frame and sends it
 * 
 * @param fd        File descriptor of the serial port
 * @param attempts  Number of attempts (>= 1)
 * @param timer     Time in seconds for time out (0 means that it'll never time out)
 * @param packet    Packet to be sent
 * @param length    Length of the packet to be sent
 * @return int      Returns -1 on error, 0 when it's successful
 */
int transmitPacket(int fd, int attempts, int timer, char *packet, int length);

#endif // TRANSMITTER_H
