#ifndef AUXILIARY_H
#define AUXILIARY_H

#define T_FILE_SIZE 0
#define T_FILE_NAME 1

int pathToFilename(char *filename, char *path, int pathLength);

int createControlPacket(char *buffer, char control, int size, char *fileName, char fileNameSize);

int createDataPacket(char *buffer, char sequence, char *data, int dataLength);

int parseControlPacket(char *packet, int packetLength, char *fileName);

#endif // AUXILIARY_H