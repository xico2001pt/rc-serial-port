#ifndef TRANSMITTER_H
#define TRANSMITTER_H

int connectTransmitter(int port);

int disconnectTransmitter(int fd);

//---------------------------------- Auxiliary Functions ----------------------------------

int communicateFrame(int fd, int attempts, int timer, char *frame, int size);

/**
 * @brief Creates supervisory/unnunbered frames
 * 
 * @param frame     Array of chars to be modified with the correct formatation
 * @param control   Control byte (can be SET, DISC, UA, RR or REJ)
 */
void createSUFrame(char *frame, char control);

/**
 * @brief Creates information frames
 * 
 * @param frame     Array of chars to be modified with the correct formatation
 * @param control   Control byte (can be SET, DISC, UA, RR or REJ)
 * @param data      Array containing the data to be transmitted
 * @param length    Number of bytes in the data array
 */
void createIFrame(char *frame, char control, char *data, int length);

#endif // TRANSMITTER_H
