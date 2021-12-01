#ifndef RECEIVER_H
#define RECEIVER_H

/**
 * @brief Settles the connection between transmitter and reciever
 * 
 * @param port  Number of port of the serial port
 * @return int  Returns -1 on error, fd of serial port when it's successful
 */
int connectReceiver(int port);

/**
 * @brief Disconnects the connection between transmitter and reciever
 * 
 * @param fd    File descriptor of the serial port
 * @return int  Returns -1 on error, 0 when it's successful
 */
int disconnectReceiver(int fd);

/**
 * @brief Receives a frame and deformats it into a packet
 * 
 * @param fd        File descriptor of the serial port
 * @param attempts  Number of attempts (>= 1)
 * @param timer     Time in seconds for time out (0 means that it'll never time out)
 * @param packet    Packet to be received
 * @return int      Returns the length of the received packet or -1 on error
 */
int recievePacket(int fd, int attempts, int timer, char *packet);

#endif // RECEIVER_H
