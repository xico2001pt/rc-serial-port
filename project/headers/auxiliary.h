#ifndef AUXILIARY_H
#define AUXILIARY_H

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

#endif // AUXILIARY_H