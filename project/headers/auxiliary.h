#ifndef AUXILIARY_H
#define AUXILIARY_H

#define T_FILE_SIZE 0
#define T_FILE_NAME 1

/**
 * @brief Extracts the filename from a path
 * 
 * @param filename      Filename extracted from the path
 * @param path          Path where the filename will be extracted
 * @param pathLength    Length of the path
 * @return int          Length of the filename
 */
int pathToFilename(char *filename, char *path, int pathLength);

/**
 * @brief Create a Control Packet with the given parameters
 * 
 * @param buffer        Buffer where the packet will be stored
 * @param control       Control byte
 * @param size          Size of the file (bytes)
 * @param fileName      Name of the file
 * @param fileNameSize  Size of the filename
 * @return int          Size of the packet
 */
int createControlPacket(char *buffer, char control, int size, char *fileName, char fileNameSize);


/**
 * @brief Create a Data Packet with the given parameters
 * 
 * @param buffer        Buffer where the packet will be stored
 * @param sequence      Sequence of the data packet (mod 256)
 * @param data          Data to be appended
 * @param dataLength    Length of the data (bytes)
 * @return int          Size of the packet
 */
int createDataPacket(char *buffer, char sequence, char *data, int dataLength);

/**
 * @brief Parses a Control packet
 * 
 * @param packet        Control packet
 * @param packetLength  Length of the packet
 * @param fileName      Name of the file
 * @return int Length of the file (bytes)
 */
int parseControlPacket(char *packet, int packetLength, char *fileName);

/**
 * @brief Outputs the control packet to the console in a human readable format 
 * 
 * @param packet    Control packet to be printed
 */
void printControlPacket(char *packet);

#endif // AUXILIARY_H